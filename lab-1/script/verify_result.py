import argparse
import numpy

def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('first_matrix', type=str, help="файл с первой матрицей")
    parser.add_argument('second_matrix', type=str, help="файл со второй матрицей")
    parser.add_argument('cpp_result', type=str, help="файл с результатом параллельного перемножения")
    args = parser.parse_args()

    a = numpy.loadtxt(args.first_matrix, skiprows=1)
    b = numpy.loadtxt(args.second_matrix, skiprows=1)
    cpp_res = numpy.loadtxt(args.cpp_result, skiprows=1)

    np_res = a @ b

    if numpy.allclose(cpp_res, np_res, atol=0.02):
        print(1)
    else:
        print(0)

if __name__ == "__main__":
    main()