/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BitcoinExchange.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/03 21:54:29 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/26 16:09:39 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include <map>
# include <string>

class BitcoinExchange
{
	public:
		BitcoinExchange();
		BitcoinExchange(const std::string &db_path);
		BitcoinExchange(const BitcoinExchange &other);
		BitcoinExchange &operator=(const BitcoinExchange &other);
		~BitcoinExchange();
		
		double get_rate(const std::string &date) const;
	
	private:
		std::map<std::string, double> rates; //on utilise map parce qu'on peut mettre les dates en clé et les rate en valeur associée

		void load_db(const std::string &db_path);
		bool is_valid_date(const std::string &date) const;
		bool is_leap_year(int year) const;
};
#endif
