from dataclasses import asdict, dataclass
import json
import os

DATA_FILE = "Vvodnya/library_catalog.json"

@dataclass
class Book:
    title: str
    author: str
    genre: str
    copies: int

def save_catalog_to_file(
    catalog: list[Book], filename: str = DATA_FILE
) -> bool:
    """Сохраняет текущий каталог книг в файл."""
    try:
        data = [asdict(book) for book in catalog]
        with open(filename, "w", encoding="utf-8") as file:
            json.dump(data, file, ensure_ascii=False, indent=4)
        print(f"[✓] Каталог успешно сохранён в файл '{filename}'.")
        return True
    except OSError as e:
        print(f"[!] Ошибка при записи в файл '{filename}': {e}")
        return False

def load_catalog_from_file(filename: str = DATA_FILE) -> list[Book]:
    """Загружает список книг из файла отдельной функцией.

    Если файл не найден или повреждён — возвращает пустой список.
    """
    if not os.path.exists(filename):
        return []

    try:
        with open(filename, "r", encoding="utf-8") as file:
            raw_data = json.load(file)

        loaded_catalog: list[Book] = []
        for item in raw_data:
            loaded_catalog.append(
                Book(
                    title=str(item.get("title", "")),
                    author=str(item.get("author", "")),
                    genre=str(item.get("genre", "")),
                    copies=int(item.get("copies", 0)),
                )
            )
        return loaded_catalog
    except (json.JSONDecodeError, OSError, ValueError) as e:
        print(f"[!] Ошибка чтения файла '{filename}': {e}")
        return []

def get_non_negative_int(prompt: str) -> int:
    """Запрашивает у пользователя целое неотрицательное число."""
    while True:
        raw_input = input(prompt).strip()
        try:
            value = int(raw_input)
            if value < 0:
                print(
                    "Ошибка: значение не может быть отрицательным. Попробуйте снова."
                )
                continue
            return value
        except ValueError:
            print("Ошибка: введено не целое число. Попробуйте снова.")


def get_non_empty_str(prompt: str) -> str:
    """Запрашивает непустую строку."""
    while True:
        value = input(prompt).strip()
        if value:
            return value
        print("Ошибка: поле не может быть пустым.")

def display_catalog(catalog: list[Book]):
    """Выводит список книг и общее количество экземпляров."""
    if not catalog:
        print("\n" + "=" * 55)
        print("Каталог пуст (введено 0 книг).")
        print("Общее количество книг в библиотеке: 0")
        print("=" * 55)
        return

    total_copies = sum(book.copies for book in catalog)

    print("\n" + "=" * 55)
    print("СПИСОК ВСЕХ КНИГ В КАТАЛОГЕ")
    print("=" * 55)
    for idx, book in enumerate(catalog, start=1):
        print(
            f"{idx}. «{book.title}» | Автор: {book.author} | Жанр: {book.genre} | Экземпляров: {book.copies}"
        )

    print("-" * 55)
    print(f"Всего наименований (N): {len(catalog)}")
    print(f"Общее количество экземпляров в библиотеке: {total_copies}")
    print("=" * 55)

def manual_input() -> list[Book]:
    """Ручной ввод N книг пользователем."""
    n = get_non_negative_int(
        "\nВведите количество наименований книг для добавления (N): "
    )
    if n == 0:
        print("Введено N = 0. Новые книги не были добавлены.")
        return []

    new_books: list[Book] = []
    for i in range(1, n + 1):
        print(f"\n--- Ввод данных для книги #{i} ---")
        title = get_non_empty_str("Название книги: ")
        author = get_non_empty_str("Автор: ")
        genre = get_non_empty_str("Жанр: ")
        copies = get_non_negative_int("Количество экземпляров: ")
        new_books.append(
            Book(title=title, author=author, genre=genre, copies=copies)
        )

    return new_books

def main():
    # Автоматическая загрузка из файла при старте
    catalog: list[Book] = load_catalog_from_file(DATA_FILE)

    print("=== Управление библиотечным каталогом ===")
    if catalog:
        print(
            f"[i] Из файла '{DATA_FILE}' автоматически загружено книг: {len(catalog)}"
        )
    else:
        print(
            f"[i] Файл '{DATA_FILE}' не найден или пуст. Каталог инициализирован с 0 книг."
        )

    while True:
        print("\n" + "#" * 45)
        print(" МЕНЮ ПРИЛОЖЕНИЯ")
        print("#" * 45)
        print("1 — Показать текущий каталог и общее количество")
        print("2 — Добавить новые книги вручную (N книг)")
        print("3 — Сохранить каталог в файл")
        print("4 — Перезагрузить каталог из файла")
        print("5 — Очистить каталог (проверка крайнего случая: 0 книг)")
        print("0 — Выйти из приложения")

        choice = input("\nВыберите действие (0-5): ").strip()

        if choice == "1":
            display_catalog(catalog)

        elif choice == "2":
            new_books = manual_input()
            if new_books:
                catalog.extend(new_books)
                print(f"\n Успешно добавлено наименований: {len(new_books)}.")
            display_catalog(catalog)

        elif choice == "3":
            save_catalog_to_file(catalog, DATA_FILE)

        elif choice == "4":
            loaded = load_catalog_from_file(DATA_FILE)
            if loaded:
                catalog = loaded
                print(f"\n[✓] Каталог успешно перезагружен из файла '{DATA_FILE}'!")
            else:
                print(f"\n[!] Файл '{DATA_FILE}' пуст или отсутствует.")
            display_catalog(catalog)

        elif choice == "5":
            catalog.clear()
            print("\n[✓] Каталог в памяти очищен.")
            display_catalog(catalog)

        elif choice == "0":
            save_prompt = (
                input("Сохранить текущие данные в файл перед выходом? (y/n): ")
                .strip()
                .lower()
            )
            if save_prompt in ("y", "yes", "д", "да"):
                save_catalog_to_file(catalog, DATA_FILE)
            print("\nРабота завершена. До свидания!")
            break

        else:
            print("\n[!] Некорректный ввод. Выберите цифру от 0 до 5.")

if __name__ == "__main__":
    main()