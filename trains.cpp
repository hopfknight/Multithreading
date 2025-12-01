#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <chrono>
#include <ctime>

const int TRAINS = 8;

std::mutex guard;

std::string now()
{
    auto t = std::time(nullptr);
    std::tm local = *std::localtime(&t);
    std::ostringstream ss;
    ss << std::put_time(&local, "%H:%M:%S");
    return ss.str();
}

int random(int min_value, int max_value)
{
    return min_value + (std::rand() % (max_value - min_value + 1));
}

struct Station
{
    std::string name;
    std::binary_semaphore gate;
    Station(const std::string& stationName)
        : name(stationName), gate(1) {}
};

void train(int id, std::vector<Station*> route, int travel_min_ms, int travel_max_ms, int pause_min_ms,  int pause_max_ms)
{
    int current_station = 0;
    int direction = 1;

    while (true)
    {
        int travel_time = random(travel_min_ms, travel_max_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(travel_time));

        Station* st = route[current_station];
        st->gate.acquire();

        {
            std::lock_guard<std::mutex> lock(guard);
            std::cout << "[" << now() << "] " << "Train " << id << " arrived at " << st->name << " (" << (direction == 1 ? "->" : "<-") << ")\n";
        }

        int pause_time = random(pause_min_ms, pause_max_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(pause_time));

        {
            std::lock_guard<std::mutex> lock(guard);
            std::cout << "[" << now() << "] " << "Train " << id << " departed from " << st->name << "\n";
        }

        st->gate.release();

        current_station += direction;

        if (current_station == (int)route.size())
        {
            direction = -1;
            current_station = route.size() - 2;
        }
        else if (current_station < 0)
        {
            direction = 1;
            current_station = 1;
        }
    }
}

int main()
{
    Station s1("DUSHANBE");
    Station s2("VAHDAT");
    Station s3("YOVON");
    Station s4("BOKHTAR");
    Station s5("DANGHARA");
    Station s6("KULOB");

    std::vector<Station*> route{ &s1, &s2, &s3, &s4, &s5, &s6 };

    std::vector<std::thread> trains;

    {
        std::lock_guard<std::mutex> lock(guard);
        std::cout << "[RAILWAY]\n";
        std::cout << "DUSHANBE -> VAHDAT -> YOVON -> BOKHTAR -> DANGHARA -> KULOB\n";
        std::cout << "NUMBER OF TRAINS: " << TRAINS << "\n\n";
    }

    for (int i = 0; i < TRAINS; i++)
    {
        trains.push_back(std::thread(train, i + 1, route, 2500, 4500, 1500, 3000));
    }

    for (auto& t : trains)
    {
        t.join();
    }

    return 0;
}
