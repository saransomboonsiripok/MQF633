#include "Date.h"

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

// Comparison Functions
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