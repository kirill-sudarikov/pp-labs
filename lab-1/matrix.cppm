export module matrix;
import std;

export template <typename T>
class square_matrix final {
private:
  std::vector<T> _data;
  int _size{0};

public:
  square_matrix(const int n) : _data(n * n), _size(n) {}

  int size() const { return _size; }

  T &operator[](const int i, const int j) { return _data[i * _size + j]; }

  T operator[](const int i, const int j) const { return _data[i * _size + j]; }

  void save_binary(const std::string &filename) const {
    std::ofstream fs(filename, std::ios::binary);

    if (!fs.is_open()) {
      throw std::runtime_error("не удалось открыть файл для записи: " +
                               filename);
    }

    fs.write(reinterpret_cast<const char *>(&_size), sizeof(_size));
    fs.write(reinterpret_cast<const char *>(_data.data()),
             _data.size() * sizeof(T));

    if (!fs) {
      throw std::runtime_error("не удалось записать матрицу в файл: " +
                               filename);
    }
  }

  void load_binary(const std::string &filename) {
    std::ifstream fs(filename, std::ios::binary);

    if (!fs.is_open()) {
      throw std::runtime_error("не удалось открыть файл для чтения: " +
                               filename);
    }

    int new_size = 0;

    fs.read(reinterpret_cast<char *>(&new_size), sizeof(new_size));

    if (new_size <= 0 || !fs) {
      throw std::runtime_error("некорректный файл: " + filename);
    }

    _size = new_size;
    _data.resize(_size * _size);

    fs.read(reinterpret_cast<char *>(_data.data()), _data.size() * sizeof(T));

    if (!fs) {
      throw std::runtime_error("некорректный файл: " + filename);
    }
  }
};

export template <typename T>
T row_x_col(const square_matrix<T> &a, const int ai, const square_matrix<T> &b,
            const int bj) {
  T res = T{0};

  for (int k = 0; k < a.size(); ++k) {
    res += a[ai, k] * b[k, bj];
  }

  return res;
}

export template <typename T>
void multiply(const square_matrix<T> &a, const square_matrix<T> &b,
              square_matrix<T> &res) {
  for (int i = 0; i < res.size(); ++i) {
    for (int j = 0; j < res.size(); ++j) {
      res[i, j] = row_x_col(a, i, b, j);
    }
  }
}

export template <typename T>
void print_matrix(const square_matrix<T> &m) {
  std::println("{}", m.size());

  for (int i = 0; i < m.size(); ++i) {
    for (int j = 0; j < m.size(); ++j) {
      std::print("{} ", m[i, j]);
    }
    std::println();
  }
}