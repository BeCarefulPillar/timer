
#include <iostream>
#include <set>
#include <map>
#include <windows.h>
#include "time_wheel.h"

void testTimer() {
    int maxMs = 1000;
    Timer t;
    t.init(maxMs);


    std::map<int, int> params;
    int num = 0;
    int a = 100;
    srand((unsigned)time(NULL));
    while (num < 1200000) {
        int dtime = (rand() % 125) * 1000 + rand() % 1000;

        if (params.find(dtime / maxMs) == params.end()) {
            params[dtime / maxMs] = 0;
        }
        params[dtime / maxMs]++;
        t.addOnce(dtime, [&a]() { a++; });
        num++;
    }

    for (auto param : params)
    {
        std::cout << param.first << " " << param.second << std::endl;
    }


    /*  int a = 100;

      t.addOnce(50, [&a]() {std::cout << "cb 50 " << a << std::endl; });

      t.addOnce(150, [&a]() {std::cout << "cb 150 " << a << std::endl; });
      t.addOnce(251, [&a]() {std::cout << "cb 251 " << a << std::endl; });
      t.addOnce(341, [&a]() {std::cout << "cb 341 " << a << std::endl; });
      t.addOnce(332, [&a]() {std::cout << "cb 332 " << a << std::endl; });

      t.addOnce(0 * 1000, [&a]() {std::cout << "cb 0 "<< a << std::endl; });
      t.addOnce(10 * 1000,[]() {std::cout << "cb 10" << std::endl; });
      t.addOnce(51 * 1000, []() {std::cout << "cb 51" << std::endl; });
      t.addOnce(66 * 1000, []() {std::cout << "cb 66" << std::endl; });
      t.addOnce(120 * 1000, []() {std::cout << "cb 120" << std::endl; });
      t.addOnce(128 * 1000, []() {std::cout << "cb 120" << std::endl; });
      t.addRepeat(4 * 1000, []() {std::cout << "cb Repeat 4" << std::endl; });
      t.addRepeat(7 * 1000, []() {std::cout << "cb Repeat 7" << std::endl; }, false);*/

    int index = maxMs;
    int time = 0;
    while (1) {
        while (index / maxMs > 0) {
            //if (time == 2) {
            //    t.addOnce(15 * 1000, []() {std::cout << "cb 2 15" << std::endl; });
            //}
            //if (time == 20) {
            //    t.addOnce(123 * 1000, []() {std::cout << "cb 20 123" << std::endl; });
            //}
            //if (time == 110) {
            //    t.addOnce(111 * 1000, []() {std::cout << "cb 110 111" << std::endl; });
            //}

            std::cout << time << std::endl;
            time++;
            index -= maxMs;
        }

        index += 30;
        t.tick(30);
        Sleep(30);
    }
}


int main()
{
    testTimer();
    return -1;
};