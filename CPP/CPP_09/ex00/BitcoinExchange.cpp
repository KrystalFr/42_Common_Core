/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 21:54:17 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/26 17:05:20 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const std::string &db_path)
{
	load_db(db_path);
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) : rates(other.rates) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
		rates = other.rates;
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}

bool BitcoinExchange::is_leap_year(int year) const
{
	if (year % 400 == 0)
		return true;
	if (year % 100 == 0)
		return false;
	return (year % 4 == 0);
}

bool BitcoinExchange::is_valid_date(const std::string &date) const
{
	if (date.size() != 10 || date[4] != '-' || date[7] != '-')
		return false;

	for (size_t i = 0; i < date.size(); ++i)
	{
		if (i == 4 || i == 7)
			continue;
		if (date[i] < '0' || date[i] > '9')
			return false;
	}

	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	if (year < 0 || month < 1 || month > 12 || day < 1)
		return false;

	static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int max = days_in_month[month - 1];
	if (month == 2 && is_leap_year(year))
		max = 29;
	return day <= max;
}

void BitcoinExchange::load_db(const std::string &path)
{
	std:: ifstream file(path.c_str());
	if (!file.is_open())
		throw std::runtime_error("Error: could not open database file.");

	std::string line;
	if (!std::getline(file, line)) // header
		throw std::runtime_error("Error: empty database file.");

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::string::size_type comma_pos = line.find(',');
		if (comma_pos == std::string::npos)
			continue;

		std::string date = line.substr(0, comma_pos);
		std::string val_str = line.substr(comma_pos + 1);

		if (!is_valid_date(date))
			continue;

// transforme val_str en flux pour en extraire un nombre et le mettre dans val, qui sera ensuite mis dans rate avec la date comme clé
		std::stringstream ss(val_str);
		double val;
		if (!(ss >> val))
			continue;
		rates[date] = val;
	}

	if (rates.empty())
		throw std::runtime_error("Error: database contains no rates.");
}

double BitcoinExchange::get_rate(const std::string &date) const
{
	if (!is_valid_date(date))
		throw std::invalid_argument("bad date");

	std::map<std::string, double>::const_iterator it = rates.lower_bound(date);
	if (it == rates.end())
	{
		--it;
		return it->second;
	}
	if (it->first == date)
		return it->second;
	if (it == rates.begin())
		throw std::out_of_range("no previous date");
	--it;
	return it->second;
}