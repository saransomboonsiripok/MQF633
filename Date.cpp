#include "Date.h"
#include <sstream>

// Function 1
double operator-(const Date& d1, const Date& d2)
{
  int yearDiff = d1.year - d2.year;
  int monthDiff = (d1.month - d2.month);
  int dayDiff = d1.day - d2.day;
  return yearDiff + monthDiff / 12.0 + dayDiff / 365.0;
}

// Function 2
std::ostream& operator<<(std::ostream& os, const Date& d)
{
  os << d.year << " " << d.month << " " << d.day << std::endl;
  return os;
}

// Function 3
std::istream& operator>>(std::istream& is, Date& d)
{
  is >> d.year >> d.month >> d.day;
  return is;
}

// Comparison operators
bool Date::operator<=(const Date& other) const {
    return (year < other.year) || (year == other.year && month < other.month) ||
           (year == other.year && month == other.month && day <= other.day);
}

bool Date::operator>=(const Date& other) const {
    return (year > other.year) || (year == other.year && month > other.month) ||
           (year == other.year && month == other.month && day >= other.day);
}

bool Date::operator<(const Date& other) const {
    return (year < other.year) || (year == other.year && month < other.month) ||
           (year == other.year && month == other.month && day < other.day);
}

bool Date::operator>(const Date& other) const {
    return (year > other.year) || (year == other.year && month > other.month) ||
           (year == other.year && month == other.month && day > other.day);
}

bool Date::operator==(const Date& other) const {
    return (year == other.year && month == other.month && day == other.day);
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

std::string Date::toString() const {
    std::ostringstream oss;
    if (year == 0 && month == 0 && day == 0) {
        return "ON";
    } else if (year == 0 && day == 0) {
        if (month == 1) {
            return "1-month";
        } else if (month % 12 == 0) {
            int years = month / 12;
            if (years == 1) {
                return "1-year";
            } else {
                oss << years << "-years";
                return oss.str();
            }
        } else {
            oss << month << "-months";
            return oss.str();
        }
    } else {
        oss << year << "-" << month << "-" << day;
        return oss.str();
    }
}

