from pathlib import Path
import csv
import os
import subprocess
import statistics
import sys

matrix_sizes = [200, 400, 800, 1200, 1600, 2000]
threads_count = [1, 2, 4, 8]
runs_count = 5

project_root = Path(__file__).resolve().parents[2]
executable = f"{project_root}\\build\\msvc-release\\lab-2\\lab-2.exe"
verify_script_path = f"{project_root}\\lab-2\\scripts\\verify_result.py"
output_csv = f"{project_root}\\benchmark_results.csv"
if not os.path.exists(f"{project_root}\\data"):
    os.makedirs(f"{project_root}\\data")

print("=== СТАРТ АВТОМАТИЧЕСКИХ ТЕСТОВ (ПО 5 ЗАПУСКОВ) ===")
print(f"{'Размер':<10} | {'1 поток':<12} | {'2 потока':<12} | {'4 потока':<12} | {'8 потоков':<12} | {'Верификация':<12}")
print("-" * 80)

with open(output_csv, mode='w', newline='', encoding='utf-8') as csv_file:
    csv_writer = csv.writer(csv_file, delimiter=';')

    csv_writer.writerow([
        "Размер матрицы", "Потоки", "Попытка 1 (мс)",
        "Попытка 2 (мс)", "Попытка 3 (мс)", "Попытка 4 (мс)", "Попытка 5 (мс)",
        "Медиана (мс)", "Результат верификации"
    ])

    for size in matrix_sizes:
        row_console_results = []
        is_verify_passed = True

        for threads in threads_count:
            a_bin = f"{project_root}\\data\\a_{size}.bin"
            b_bin = f"{project_root}\\data\\b_{size}.bin"
            r_bin = f"{project_root}\\data\\r_{size}.bin"

            if not os.path.exists(a_bin) or not os.path.exists(b_bin):
                row_console_results.append(f"файлы не найдены: {a_bin}, {b_bin}, {r_bin}")
                csv_writer.writerow([f"{size}x{size}", threads, f"файлы не найдены: {a_bin}, {b_bin}, {r_bin}"])
                continue

            cmd = [
                executable, "multiply",
                a_bin, b_bin, r_bin,
                "--threads", str(threads)
            ]

            test_times = []

            for i in range(runs_count):
                passed = False
                try:
                    result = subprocess.run(cmd, capture_output=True, text=True, encoding='utf-8')

                    for line in result.stdout.splitlines():
                        if line.startswith("time_ms="):
                            time = float(line.split("=")[1])
                            test_times.append(time)
                            break

                    file_name = f"np_res_{size}.bin"

                    found_file = next(project_root.rglob(file_name), None)
                    
                    if not found_file:
                        result = subprocess.run(
                            [
                                sys.executable, verify_script_path, "multiply_verify",
                                a_bin, b_bin, r_bin
                            ],
                            capture_output=True,
                            text=True,
                            encoding='utf-8')

                        passed = result.returncode == 0
                    else:
                        result = subprocess.run(
                            [
                                sys.executable, verify_script_path, "verify",
                                r_bin, str(found_file)
                            ],
                            capture_output=True,
                            text=True,
                            encoding='utf-8')

                        passed = result.returncode == 0

                    if passed == False:
                        is_verify_passed = False
                except Exception:
                    pass

            if len(test_times) == runs_count:
                median_time = statistics.median(test_times)
                row_console_results.append(f"{median_time:.2f} мс")

                csv_writer.writerow([
                    f"{size}x{size}", threads,
                    f"{test_times[0]:.4f}", f"{test_times[1]:.4f}", f"{test_times[2]:.4f}", 
                    f"{test_times[3]:.4f}", f"{test_times[4]:.4f}", f"{median_time:.4f}",
                    is_verify_passed
                ])
            else:
                row_console_results.append("Ошибка")
                csv_writer.writerow([f"{size}x{size}", threads, "Ошибка запуска"])

        print(f"{f'{size}x{size}':<10} | {row_console_results[0]:<12} | {row_console_results[1]:<12} | {row_console_results[2]:<12} | {row_console_results[3]:<12} | {is_verify_passed:<12}")

print("-" * 80)
print(f"=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ. Результаты сохранены в {output_csv} ===")
