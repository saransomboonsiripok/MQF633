#ifndef DATE_H
#define DATE_H

#include <iostream>

class Date
{
 public:
  int year;
  int month;
  int day;
  Date(int y, int m, int d) : year(y), month(m), day(d) {};
  Date(){};

  // Comparison operators
  bool operator<=(const Date& other) const;
  bool operator>=(const Date& other) const;
  bool operator<(const Date& other) const;
  bool operator>(const Date& other) const;
  bool operator==(const Date& other) const;
  bool operator!=(const Date& other) const;
  std::string toString() const;
};

// declaring functions
double operator-(const Date& d1, const Date& d2);
std::ostream& operator<<(std::ostream& os, const Date& date);
std::istream& operator>>(std::istream& is, Date& date);

#endif