//
// Created by NANAnoo on 2/11/2023.
//
#pragma once

#include <ctime>
#include <memory>
#include "SingletonBase.h"

namespace SPW {
    // a duration
    class TimeDuration {
    public:
        double value;
        static TimeDuration second() {
            TimeDuration d{};
            d.value = 1;
            return d;
        }
        static TimeDuration second(double num) {
            TimeDuration d{};
            d.value = num;
            return d;
        }
        static TimeDuration ms() {
            return second() * 1000;
        }
        static TimeDuration ms(double num) {
            return ms() * num;
        }
        static TimeDuration us() {
            return second() * 1000 * 1000;
        }
        static TimeDuration us(double num) {
            return us() * num;
        }
        static TimeDuration ns() {
            return second() * 1000 * 1000 * 1000;
        }
        static TimeDuration ns(double num) {
            return ns() * num;
        }

        [[nodiscard]] double toSecond() const {return value;}
        [[nodiscard]] double toMs() const {return 1000 * value;}
        [[nodiscard]] double toUs() const {return 1000 * 1000 * value;}
        [[nodiscard]] double toNs() const {return 1000 * 1000 * 1000 * value;}

        TimeDuration operator *(const double &num) {
            TimeDuration d = second();
            d.value *= num;
            return d;
        }

        TimeDuration operator -(TimeDuration &other) const {
            TimeDuration d{};
            d.value = value - other.value;
            return d;
        }

        TimeDuration operator +(TimeDuration &other) const {
            TimeDuration d{};
            d.value = value + other.value;
            return d;
        }

        TimeDuration operator /(const float &num) {
            TimeDuration d = second();
            d.value /= num;
            return d;
        }
        //friend std::ostream& operator<<(std::ostream& os, const TimeDuration& td);
    };
    // a time stamp
    class TimeStamp {
    public:
        TimeDuration operator -(const TimeStamp &other) const {
            TimeDuration d{};
            d.value = value - other.value;
            return d;
        }
        double value;
        //friend std::ostream& operator<<(std::ostream& os, const TimeStamp& ts);

    };


    class TimerI {
    public:
        virtual TimeStamp current() = 0;
        virtual ~TimerI() = default;
    };

    // get time from ctime
    class DefaultTimer : public TimerI {
    public:
         TimeStamp current() override {
            TimeStamp ts{};
            ts.value = static_cast<double >(std::clock()) / CLOCKS_PER_SEC;
            return ts;
        }
        explicit DefaultTimer() = default;
         ~DefaultTimer() override = default;
    };

    static std::unique_ptr<SPW::TimerI> sTimer =std::make_unique<DefaultTimer>();
    namespace Timer {
        static void setupTimer(std::unique_ptr<TimerI> &timer) {
            sTimer = std::move(timer);
        }
        static TimerI & timer() {
            return *sTimer;
        }
        static TimeStamp current() {
            return timer().current();
        }
    }
}

//std::ostream& operator << (std::ostream &os, const SPW::TimeStamp &ts) {
//    os << std::to_string(ts.value);
//    os << std::string("/s");
//    return os;
//}
//std::ostream& operator << (std::ostream &os, const SPW::TimeDuration &td) {
//    os << std::to_string(td.value);
//    os << std::string("/s");
//    return os;
//}
