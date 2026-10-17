import argparse
import numpy as np
import sys
import os

def load_matrix(filename: str) -> np.ndarray:
    with open(filename, "rb") as file:
        n = np.fromfile(file, dtype=np.int32, count=1)[0]
        data = np.fromfile(file, dtype=np.float64, count=n * n)
    return data.reshape(n, n)

def save_matrix(filename: str, matrix: np.ndarray) -> None:
    with open(filename, "wb") as file:
        np.array([matrix.shape[0]], dtype=np.int32).tofile(file)
        np.asarray(matrix, dtype=np.float64).tofile(file)

def main() -> None:
    parser = argparse.ArgumentParser(description="Testing C++ matrix multiplication with NumPy")

    subparsers = parser.add_subparsers(dest='command',
                                       required=True, help='available modes')

    parser_mul_ver = subparsers.add_parser('multiply_verify', help='Multiply A and B, compare'
                          ' with the C++ result, and save the output using NumPy.')
    parser_mul_ver.add_argument('matrix_a', type=str, help='binary file with first matrix')
    parser_mul_ver.add_argument('matrix_b', type=str, help='binary file with second matrix')
    parser_mul_ver.add_argument('result', type=str, help='binary file with C++ result matrix')

    parser_ver = subparsers.add_parser('verify', help='Compares C++ and NumPy results')
    parser_ver.add_argument('cpp_result', type=str, help='binary file with C++ result matrix')
    parser_ver.add_argument('numpy_result', type=str, help='binary file with NumPy result matrix')

    args = parser.parse_args()

    if args.command == 'multiply_verify':
        try:
            a = load_matrix(args.matrix_a)
            b = load_matrix(args.matrix_b)
            cpp_r = load_matrix(args.result)
    
            np_r = a @ b
    
            is_equal = np.allclose(cpp_r, np_r)
    
            if not is_equal:
                max_error = np.max(np.abs(cpp_r - np_r))
                print(f"Maximum difference: {max_error:.6e}", file=sys.stderr)

            data_dir = 'data'

            if not os.path.exists(data_dir):
                os.makedirs(data_dir)

            file_path = os.path.join(data_dir, f"np_res_{a.shape[0]}.bin")
            save_matrix(file_path, np_r)

            print(is_equal)
            sys.exit(0 if is_equal else 1)
        except (OSError, ValueError) as exc:
            print(f"Error: {exc}", file=sys.stderr)
            sys.exit(2)
    elif args.command == 'verify':
        try:
            cpp_r = load_matrix(args.cpp_result)
            np_r = load_matrix(args.numpy_result)

            is_equal = np.allclose(cpp_r, np_r)
            print(is_equal)
                
            if not is_equal:
                max_error = np.max(np.abs(cpp_r - np_r))
                print(f"Maximum difference: {max_error:.6e}", file=sys.stderr)
            
            sys.exit(0 if is_equal else 1)
        except (OSError, ValueError) as exc:
            print(f"Error: {exc}", file=sys.stderr)
            sys.exit(2)

if __name__ == "__main__":
    main()