/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScalarConverter.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 17:18:04 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/06 05:21:08 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCALARCONVERTER_H
# define SCALARCONVERTER_H

#include <string>
#include <iostream>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <cerrno>

class ScalarConverter
{
	private:
		ScalarConverter();
		ScalarConverter(const ScalarConverter& other);
		ScalarConverter& operator=(const ScalarConverter& other);
		~ScalarConverter();
	
	public:
		static void convert(const std::string& input);
		
		class invalidInput : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		class Overflow : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
};

#endif