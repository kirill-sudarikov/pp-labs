import argparse
import numpy as np

def load_matrix(filename: str) -> np.ndarray:
    with open(filename, "rb") as file:
        n = np.fromfile(file, dtype=np.int32, count=1)[0]
        data = np.fromfile(file, dtype=np.float64, count=n * n)
    return data.reshape(n, n)

def main() -> None:
    parser = argparse.ArgumentParser(description="Testing C++ matrix multiplication with NumPy")

    parser.add_argument('matrix_a', type=str, help='binary file with first matrix')
    parser.add_argument('matrix_b', type=str, help='binary file with second matrix')
    parser.add_argument('result', type=str, help='binary file with C++ result matrix')

    args = parser.parse_args()

    try:
        a = load_matrix(args.matrix_a)
        b = load_matrix(args.matrix_b)
        cpp_r = load_matrix(args.result)

        np_r = a @ b

        is_equal = np.allclose(cpp_r, np_r)

        print(f"A size: {a.shape}")
        print(f"B size: {b.shape}")
        print(f"C size (C++): {cpp_r.shape}")
        print(f"C size (NumPy): {np_r.shape}")

        if is_equal:
            print("Verification passed: results match.")
        else:
            max_error = np.max(np.abs(cpp_r - np_r))

            print("Verification failed.")
            print(f"Maximum difference: {max_error:.6e}")
    except (FileNotFoundError, ValueError) as exc:
        print(f"Error: {exc}")

if __name__ == "__main__":
    main()