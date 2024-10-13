/*
    Программа предназначена решать задачу на растяжение-сжатие по сопромату.
    Это первый черновой вариант, дальше планируется его улучшение
    - выведение некоторых частей кода в функции, создание клаассов и графической части.

    Слудующие шаги:
    (сделано) - добавить вторую заделку
    - добавить распределённую нагрузку, в том числе и в виде веса
    - создать функция для вывода результатов, вынести повторяющийся код
    - создать классы для решения отдельных задач
*/

#include <iostream>
#include <cmath>

//                const vector<double>& innerForces
void printResults(double* innerForces, double* sigmas, double* deltaLens, double* displacements, int n, double sigmaMax, double sigmaT, double A, double U)
{
    std::cout << "=====================================================================\n\n";
    std::cout << "Вывод результатов при движении от свободного конца слева" << std::endl;
    std::cout << "\n\n";
        
    std::cout << "Значения внутренних нормальных сил [кН]: " << std::endl;
    for (int i = 0; i < n; i++)
    {
        std::cout << innerForces[i] << ",\t";
    }
    std::cout << "\n\n";

    std::cout << "Значения нормальных напряжений [МПа]: " << std::endl;
    for (int i = 0; i < n; i++)
    {
        std::cout << sigmas[i] << ",\t";
    }
    std::cout << "\n\n";

    std::cout << "Значения удлинений участков [10^(-4) м]: " << std::endl;
    for (int i = 0; i < n; i++)
    {
        std::cout << deltaLens[i] << ",\t";
    }
    std::cout << "\n\n";

    std::cout << "Значения перемещений поперечных сечений участков [10^(-4) м]: " << std::endl;
    for (int i = 0; i < n + 1; i++)
    {
        std::cout << displacements[i] << ",\t";
    }
    std::cout << "\n\n\n";

    std::cout << "Коэффициент запаса прочности по текучести n=" << sigmaMax / sigmaT;
    std::cout << "\n\n";

    std::cout << "Работа внешних сил A = " << A;
    std::cout << "\n\n";

    std::cout << "Энергия деформирования U = " << U;
    std::cout << "\n\n";
}

int main()
{
    setlocale(LC_ALL, "ru");
    int n; // Количество участков стержня
    double modulE; // Модуль упругости
    int fixersNumber = 0; // Количество заделок по концам стержня
    double sigmaMax = 0;
    double sigmaT;
    double A = 0;
    double U = 0;
    //int resultParts; 
    //resultParts = 2;
    //const double Pi = 3.14159;

    while ((fixersNumber != 1) && (fixersNumber != 2))
    {
        std::cout << "Введите количество заделок стержня (1 или 2) :";
        std::cin >> fixersNumber;
    }

    std::cout << "Введите модуль упругости E [МПа] :";
    std::cin >> modulE;
    std::cout << "Введите предел прочности [МПа] :";
    std::cin >> sigmaT;
    std::cout << "Введите количество участвок стержня: ";
    std::cin >> n;

    double* lengths = new double[n]; // vector<double> lengths(n);
    double* areas = new double[n];
    double* forces = new double[n];
    double* innerForces = new double[n];
    double* sigmas = new double[n];
    double* deltaLens = new double[n];
    double* displacements = new double[n + 1] {};// Перемещения поперечных сечений в узлах(концах участков) стержня

    /*for (int i = 0; i <= n; i++)
    {
        displacements[i] = 0.0;
    }*/

    // Ввод исходных данных
    for (int i = 0; i < n; i++)
    {
        std::cout << "Введите длину участка " << i + 1 << " [м] : ";
        std::cin >> lengths[i];

        std::cout << "Введите площадь поперечного сечения на участке " << i + 1 << " [см^2] : ";
        std::cin >> areas[i];

        std::cout << "Введите внушнюю силу в начале участка " << i + 1 << " [кН] : ";
        std::cin >> forces[i];
        innerForces[i] = forces[i];
    }

    // Определение внутренних нормальных сил, нормальных напряжений и удлинений участков
    for (int i = 0; i < n; i++)
    {
        if (i != 0)
        {
            innerForces[i] += innerForces[i - 1];
        }

        sigmas[i] = innerForces[i] / areas[i] * 10;
        deltaLens[i] = innerForces[i] * lengths[i] * 100000 / (modulE * areas[i]);
    }

    // Определение перемещений поперечных сеений
    for (int i = n - 1; i >= 0; i--)
    {
        displacements[i] = displacements[i + 1] + deltaLens[i];
    }

    // Определение Fx
    // 1. Аналитически
    double dispTotal = displacements[0];
    //std::cout << " dispTotal = " << dispTotal;
    double dispFx = 0;
    for (int i = 0; i < n; i++)
    {
        dispFx += lengths[i] * 100000 / (modulE * areas[i]);
    }
    //std::cout << " dispFx = " << dispFx;
    double Fx = -dispTotal / dispFx;
    // 2. Перевобом

    std::cout << " Fx = " << Fx;


    if (fixersNumber == 2)
    {
        for (int i = 0; i < n; i++)
        {
            innerForces[i] += Fx;
            sigmas[i] = innerForces[i] / areas[i] * 10;
            deltaLens[i] = innerForces[i] * lengths[i] * 100000 / (modulE * areas[i]);
        }
        for (int i = n - 1; i >= 0; i--)
        {
            displacements[i] = displacements[i + 1] + deltaLens[i];
        }
    }

    // Определение максимального напряжения
    for (int i = 0; i < n; i++)
    {
        if (fabs(sigmas[i]) > sigmaMax)
            sigmaMax = fabs(sigmas[i]);
    }

    //Определение работы внешних сил
    for (int i = 0; i < n; i++)
    {
        A += 0.5 * forces[i] * displacements[i];
    }
    A /= 10;

    //Определение потенциальной энергии деформаций
    for (int i = 0; i < n; i++)
    {
        U += 0.5 * innerForces[i] * deltaLens[i];
    }
    U /= 10;

    // Вывод результатов
    printResults(innerForces, sigmas, deltaLens, displacements, n, sigmaMax, sigmaT, A, U);

    delete[] displacements;
    delete[] deltaLens;
    delete[] sigmas;
    delete[] innerForces;
    delete[] forces;
    delete[] areas;
    delete[] lengths;
}
