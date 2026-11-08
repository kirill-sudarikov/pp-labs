export module random;
import std;
import matrix;

export square_matrix<double> make_random_matrix(const int size,
                                                const double min_value,
                                                const double max_value,
                                                const std::size_t seed) {
  if (min_value >= max_value) {
    throw std::invalid_argument(
        std::format("нижняя граница диапазона больше верхней: {} > {}",
                    min_value, max_value));
  }

  std::mt19937 prng(static_cast<std::mt19937::result_type>(seed));
  std::uniform_real_distribution<double> d(min_value, max_value);

  square_matrix<double> m(size);

  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < size; ++j) {
      m[i, j] = d(prng);
    }
  }

  return m;
}