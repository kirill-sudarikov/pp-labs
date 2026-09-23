import std;
import matrix;
import random;

int main(int argc, char *argv[]) {
  try {
    const std::string_view command = argc > 1 ? argv[1] : "";

    if (command == "multiply") {
      if (argc != 4) {
        throw std::invalid_argument(
            "использование: lab-1 multiply <a.bin> <b.bin>");
      }

      square_matrix<double> a(1);
      square_matrix<double> b(1);

      a.load_binary(argv[2]);
      b.load_binary(argv[3]);

      if (a.size() != b.size()) {
        throw std::invalid_argument("матрицы должны быть одного порядка");
      }

      square_matrix<double> result(a.size());

      const auto start = std::chrono::steady_clock::now();
      multiply(a, b, result);
      const auto end = std::chrono::steady_clock::now();

      const auto elapsed_time = end - start;

      result.save_binary("result.bin");

      std::println("Размер матриц: {} x {}", a.size(), a.size());
      std::println("Тип элементов: double");
      std::println("Объём задачи: {} элементов в одной матрице",
                   a.size() * a.size());
      std::println("Объём данных A, B и R: {} байт",
                   a.size() * a.size() * sizeof(double) * 3);
      std::println("Логических процессоров: {}",
                   std::thread::hardware_concurrency());
      std::println(
          "Время умножения: {:.3f} мс",
          std::chrono::duration<double, std::milli>(elapsed_time).count());

      std::println(
          "time_ms={:.6f}",
          std::chrono::duration<double, std::milli>(elapsed_time).count());
      std::println("Результат сохранён в result.bin");
    }

    if (command == "generate") {
      if (argc != 7) {
        throw std::invalid_argument("использование: lab-1 generate <size> "
                                    "<min> <max> <seed> <output.bin>");
      }

      const int size = std::stoi(argv[2]);
      const double min_value = std::stod(argv[3]);
      const double max_value = std::stod(argv[4]);
      const int seed = std::stoi(argv[5]);
      const std::string_view output = argv[6];

      auto matrix = make_random_matrix(size, min_value, max_value, seed);

      matrix.save_binary(std::string(output));

      std::println("Матрица {} x {} сохранена в {}", matrix.size(),
                   matrix.size(), output);
    }
  } catch (const std::exception &e) {
    std::cerr << "Произошла ошибка: " << e.what() << '\n';
    return 1;
  }

  return 0;
}