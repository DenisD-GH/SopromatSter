/*
    Программа предназначена решать задачу на растяжение-сжатие по сопромату.
    Это черновой вариант, дальше планируется его улучшение
    - выведение некоторых частей кода в функции, создание клаассов и графической части.

    Слудующие шаги:
    (сделано) - добавить вторую заделку
    - добавить распределённую нагрузку, в том числе и в виде веса
    (сделано) создать функция для вывода результатов, вынести повторяющийся код
    (в процессе) создать классы для решения отдельных задач


    -проверить displacements
    -перевести единицы измерения
*/



#include <iostream>
#include <vector>

class Rod {
public:
    Rod(int n, double modulE, int fixersNumber)
        : n(n), modulE(modulE), fixersNumber(fixersNumber) {
        sections.resize(n);
    }

    void setSection(int index, double area, double length, double force) {
        if (index >= 0 && index < n) {
            sections[index] = Section(area, length, force);
        }
        else {
            std::cout << "Ошибка: индекс участка вне диапазона." << std::endl;
        }
    }

    void calculateInnerForces() {
        double previousInnerForce = 0;

        for (size_t i = 0; i < sections.size(); ++i) {
            sections[i].innerForce = previousInnerForce + sections[i].force;
            previousInnerForce = sections[i].innerForce;
        }
    }

    void calculateStress() {
        for (size_t i = 0; i < sections.size(); ++i) {
            sections[i].stress =  sections[i].innerForce * 10 / sections[i].area;  // Перевод в МПа
        }
    }

    void calculateDeformation() {
        for (size_t i = 0; i < sections.size(); ++i) {
            sections[i].deformation = sections[i].innerForce * sections[i].length * 100000 / (modulE * sections[i].area);
        }
    }

    void calculateDisplacement() {
        for (size_t i = sections.size() - 1; i > 0; --i) {
            nodes[i].displacement = nodes[i+1].displacement + sections[i - 1].deformation;
        }
    }

    // Тут должна быть обработка случая с двумя заделками

    void calculateWorkAndEnergy() {
        for (int i = 0; i < n; i++) {
            A += 0.5 * sections[i].force * nodes[i].displacement;
            U += 0.5 * sections[i].innerForce * sections[i].deformation;
        }

        A /= 10;   // Перевод в Дж
        U /= 10;   // Перевод в Дж
    }

    void printInfo() const {
        std::cout << "Количество участков стержня: " << sections.size() << "\n\n";
        std::cout << "Модуль упругости: " << modulE << " МПа" << "\n\n";
        std::cout << "Количество заделок по концам: " << fixersNumber << "\n\n";

        for (size_t i = 0; i < sections.size(); ++i) {
            std::cout << "Участок " << (i + 1) << ":" << "\n\n";
            std::cout << "  Длина: " << sections[i].length << " м" << "\n\n";
            std::cout << "  Площадь: " << sections[i].area << " см^2" << "\n\n";
            std::cout << "  Внешняя сила: " << sections[i].force << " кН" << "\n\n";
            std::cout << "  Внутренняя сила: " << sections[i].innerForce << " кН" << "\n\n";
            std::cout << "  Внутреннее напряжение: " << sections[i].stress << " МПа" << "\n\n";
        }

        std::cout << "Смещения узлов:" << "\n\n";
        for (size_t i = 0; i < nodes.size(); ++i) {
            std::cout << "  Узел " << (i + 1) << ": Смещение = " << nodes[i].displacement << " м" << "\n\n";
        }

        std::cout << "Работа внешних сил A = " << A;
        std::cout << "\n\n";

        std::cout << "Энергия деформирования U = " << U;
        std::cout << "\n\n";
    }


    class Section {
    public:
        double area;       // Площадь поперечного сечения
        double length;     // Длина участка
        double force;      // Внешняя сосредоточенная сила в начале участка
        double innerForce; // Внутренняя сила
        double stress;     // Внутреннее нормальное напряжение
        double deformation;// Изменение длины

        Section(double area = 0, double length = 0, double force = 0)
            : area(area), length(length), force(force), innerForce(0), stress(0), deformation(0) {}
    };

    class Node {
    public:
        double displacement; // Продольное перемещение поперечного сечения

        Node(double displacement = 0) : displacement(displacement) {}
    };

private:
    int n;                         // Количество участков стержня
    double modulE;                 // Модуль упругости
    int fixersNumber;              // Количество заделок по концам стержня
    double A = 0.0;                // Работа внешних сил
    double U = 0.0;                // Потенциальная энергия деформирования
    std::vector<Section> sections; // Вектор для хранения участков
    std::vector<Node> nodes;       // Вектор узлов
};

int main() {
    setlocale(LC_ALL, "ru");

    int n;
    double modulE;

    // Ввод данных
    std::cout << "Введите количество участков стержня: ";
    std::cin >> n;

    std::cout << "Введите модуль упругости (МПа): ";
    std::cin >> modulE;

    int fixersNumber;
    do {
        std::cout << "Введите количество заделок (1 или 2): ";
        std::cin >> fixersNumber;

        if (fixersNumber != 1 && fixersNumber != 2) {
            std::cout << "Ошибка: необходимо ввести 1 или 2." << std::endl;
        }
    } while (fixersNumber != 1 && fixersNumber != 2);

    Rod rod(n, modulE, fixersNumber);

    for (int i = 0; i < n; ++i) {
        double length, area, force;
        std::cout << "Участок " << (i + 1) << ":" << std::endl;
        std::cout << "Введите длину (м): ";
        std::cin >> length;
        std::cout << "Введите площадь поперечного сечения (см^2): ";
        std::cin >> area;
        std::cout << "Введите величину внешней силы (кН): ";
        std::cin >> force;

        rod.setSection(i, area, length, force);
    }

    rod.calculateInnerForces();
    rod.calculateStress();
    rod.calculateDeformation();
    rod.printInfo();

    return 0;
}
