/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 21:54:17 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/24 14:04:42 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BitcoinExchange.hpp"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange()
{
	load_db(db_path);
}

BitcoinExchange::BitcointExchange(const BitcoinExchange &other) : rates(other.rates) {}

BitcoinExchange::BitcointExchange(const BitcoinExchange &other)
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