import std;
import matrix;
import random;
#include "mpi.h"

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  try {
    int rank, size;
    int n, local_n;
    double *local_a, *b_data, *local_c;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    square_matrix<double> a(1);
    square_matrix<double> b(1);

    std::string c_filename;

    if (rank == 0) {
      if (argc != 4) {
        std::cerr << "Usage: lab-3.exe <input1.bin> <input2.bin> <output.bin>"
                  << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
      }

      std::string a_filename = argv[1];
      std::string b_filename = argv[2];
      c_filename = argv[3];

      a.load_binary(a_filename);
      b.load_binary(b_filename);

      if (a.size() != b.size()) {
        std::cerr << "matrix dimensions must match" << std::endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
      }

      n = a.size();
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    local_n = n / size + (rank < n % size ? 1 : 0);

    local_a = new double[local_n * n];
    b_data = new double[n * n];
    local_c = new double[local_n * n]();

    MPI_Barrier(MPI_COMM_WORLD);
    double total_start = MPI_Wtime();

    if (rank == 0) {
      std::memcpy(b_data, b.data(), n * n * sizeof(double));
    }

    MPI_Bcast(b_data, n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    std::vector<int> sendcounts;
    std::vector<int> displs;

    if (rank == 0) {
      sendcounts.resize(size);
      displs.resize(size);

      int offset = 0;

      for (int i = 0; i < size; ++i) {
        const int rows = n / size + (i < n % size ? 1 : 0);

        sendcounts[i] = rows * n;
        displs[i] = offset;

        offset += sendcounts[i];
      }
    }

    MPI_Scatterv(a.data(), sendcounts.data(), displs.data(), MPI_DOUBLE,
                 local_a, local_n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double calc_start = MPI_Wtime();

    for (int i = 0; i < local_n; ++i) {
      for (int j = 0; j < n; ++j) {
        for (int k = 0; k < n; ++k) {
          local_c[i * n + j] += local_a[i * n + k] * b_data[k * n + j];
        }
      }
    }

    double calc_end = MPI_Wtime();
    double local_calc_time = calc_end - calc_start;

    std::vector<double> c_data;

    if (rank == 0) {
      c_data.resize(n * n);
    }

    MPI_Gatherv(local_c, local_n * n, MPI_DOUBLE, c_data.data(),
                sendcounts.data(), displs.data(), MPI_DOUBLE, 0,
                MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double total_end = MPI_Wtime();
    double local_total_time = total_end - total_start;

    double max_total_time, max_calc_time;
    MPI_Reduce(&local_total_time, &max_total_time, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);
    MPI_Reduce(&local_calc_time, &max_calc_time, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
      std::cout << "-------------------------------------------------------"
                << std::endl;
      std::cout << "Результаты замеров (Размер матрицы: " << n
                << ", Процессов: " << size << ")" << std::endl;
      std::cout << "-------------------------------------------------------"
                << std::endl
                << std::endl;

      std::cout << "Чистые вычисления (Max): " << max_calc_time << " сек."
                << std::endl;
      std::cout << "Общее время с пересылками (Max): " << max_total_time
                << " сек." << std::endl;
      std::cout << "Затраты на MPI-коммуникации: "
                << (max_total_time - max_calc_time) << " сек." << std::endl;

      square_matrix<double> c(c_data, n);
      c.save_binary(c_filename);
    }

    delete[] local_a;
    delete[] b_data;
    delete[] local_c;

  } catch (const std::exception &e) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    std::cerr << "[Rank " << rank << "] error: " << e.what() << std::endl;
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  MPI_Finalize();
  return 0;
}