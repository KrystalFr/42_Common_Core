/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ScalarConverter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 17:17:56 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/06 17:34:10 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ScalarConverter.hpp"

ScalarConverter::ScalarConverter(){}
ScalarConverter::ScalarConverter(const ScalarConverter&) {}
ScalarConverter& ScalarConverter::operator=(const ScalarConverter&) {return *this;}
ScalarConverter::~ScalarConverter() {}

const char* ScalarConverter::invalidInput::what() const throw(){
	return "Invalid input";
}

bool pseudo_float(const std::string& input)
{
	std::string pseudo[3] = {"-inff", "+inff", "nanf"};

	for (int i = 0; i < 3; i++)
	{
		if (pseudo[i] == input)
			return true;
	}
	return false;
}

bool pseudo_double(const std::string& input)
{
	std::string pseudo[3] = {"-inf", "+inf", "nan"};

	for (int i = 0; i < 3; i++)
	{
		if (pseudo[i] == input)
			return true;
	}
	return false;
}

bool isint(const std::string& input)
{
	size_t start = 0;
	if (input[0] == '+' || input[0] == '-')
		start = 1;
	for (size_t i = start; i < input.size(); i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(input[i])))
			return false;
	}
	return true;
}

bool isdouble(const std::string& input)
{
	size_t start = 0;
	int dot = 0;
	if (input[0] == '+' || input[0] == '-')
		start = 1;
	for (size_t i = start; i < input.size(); i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(input[i])))
		{
			if (input[i] != '.')
				return false;
			else
			{
				if (i == 0)
					return false;
			}
			if (i == input.size() - 1)
				return false;
			if (dot == 1)
				return false;
			else
				dot = 1;
		}
	}
	if (dot == 1)
		return true;
	else
		return false;
}

bool isfloat(const std::string& input)
{
	size_t start = 0;
	int dot = 0;
	if (input[0] == '+' || input[0] == '-')
		start = 1;
	if (input[input.size() - 1] != 'f')
		return false;
	for (size_t i = start; i < input.size(); i++)
	{
		if (!std::isdigit(static_cast<unsigned char>(input[i])) && (i != input.size() - 1))
		{
			if (input[i] != '.')
				return false;
			else
			{
				if (i == 0)
					return false;
			}
			if (input[i] == '.' && input[i + 1] == 'f')
				return false;
			if (dot == 1)
				return false;
			else
				dot = 1;
		}
	}
	return true;
}

/*
0 = NON_DISPLAY
1 = CHAR
2 = INT
3 = FLOAT
4 = DOUBLE
5 = PSEUDO_FLOAT/PSEUDO_DOUBLE
6 = INVALID
*/
int DetectType(const std::string& input)
{
	unsigned char c;
	if (input.empty())
		return 6;
	if (pseudo_float(input))
		return 5;
	if (pseudo_double(input))
		return 5;
	if (input.size() == 3)
	{
		if (input[0] == '\'' && input[2] == '\'')
		{
			c = static_cast<unsigned char>(input[1]);
			if (c > 127)
				return 6;
			if (std::isprint(c))
				return 1;
			else
				return 0;
		}
	}
	if (input.size() == 1)
	{
		c = static_cast<unsigned char>(input[0]);		
		if (c > 127)
			return 6;
		if (std::isdigit(c))
			return 2;
		if (std::isprint(c))
			return 1;
		else
			return 0;
	}
	if (isint(input))
		return 2;
	if (isdouble(input))
		return 4;
	if (isfloat(input))
		return 3;
	return 6;
}

void printPseudoFromValue(double v)
{
	std::cout << "char: impossible" << std::endl;
	std::cout << "int: impossible" << std::endl;

	if (v != v)
	{
		std::cout << "float: nanf" << std::endl;
		std::cout << "double: nan" << std::endl;
		return;
	}

	if (v < 0)
	{
		std::cout << "float: -inff" << std::endl;
		std::cout << "double: -inf" << std::endl;
	}
	else
	{
		std::cout << "float: +inff" << std::endl;
		std::cout << "double: +inf" << std::endl;
	}
}

void printPseudo(const std::string& input)
{
	if (input == "nan" || input == "nanf")
	{
		printPseudoFromValue(std::numeric_limits<double>::quiet_NaN());
		return;
	}
	if (input == "-inf" || input == "-inff")
	{
		printPseudoFromValue(-std::numeric_limits<double>::infinity());
		return;
	}
	if (input == "+inf" || input == "+inff")
	{
		printPseudoFromValue(std::numeric_limits<double>::infinity());
		return;
	}
}

static bool isIntegral (double v)
{
	 if (v != v) // NaN
        return false;
    if (v > static_cast<double>(std::numeric_limits<long>::max()) ||
        v < static_cast<double>(std::numeric_limits<long>::min()))
	{
        return false;
	}
	return (v == static_cast<double>(static_cast<long>(v)));
}

void printChar(const std::string& input, int type)
{
	char c;
	if (input.size() == 3)
		c = input[1];
	else
		c = input[0];
	if (type == 0)
		std::cout << "char: Non displayable" << std::endl;
	else
		std::cout << "char: '" << c << "'" <<std::endl;
	std::cout << "int: " << static_cast<int>(c) << std::endl;
	
	std::cout << "float: ";
	if (isIntegral(c))
		std::cout << static_cast<long>(c) << ".0f" << std::endl;
	else
		std::cout << static_cast<float>(c) << "f" << std::endl;

	std::cout << "double: ";
	if (isIntegral(c))
		std::cout << static_cast<long>(c) << ".0" << std::endl;
	else
		std::cout << c << std::endl;
}

void printDouble(const std::string& input)
{
	int min = std::numeric_limits<int>::min();
	int max = std::numeric_limits<int>::max();
	double dmin = -std::numeric_limits<double>::max();
	double dmax = std::numeric_limits<double>::max();
	errno = 0;
	double str = std::strtod(input.c_str(),NULL);

	if (errno == ERANGE)
	{
		printPseudoFromValue(str);
		return;
	}
	if(str != str || str > dmax || str < dmin)
	{
		printPseudoFromValue(str);
		return;
	}
	
	if (str < 0.0 || str > 127.0)
		std::cout << "char: impossible" << std::endl;
	else
	{
		unsigned char c = static_cast<unsigned char>(str);
		if (!std::isprint(c))
			std::cout << "char: Non displayable" << std::endl;
		else
			std::cout << "char: '" << static_cast<char>(c) << "'" << std::endl;
	}

	if (str < static_cast<double>(min) || str > static_cast<double>(max))
		std::cout << "int: impossible" << std::endl;
	else
		std::cout << "int: " << static_cast<int>(str) << std::endl;
	
	std::cout << "float: ";
	if (isIntegral(str))
		std::cout << static_cast<long>(str) << ".0f" << std::endl;
	else
		std::cout << static_cast<float>(str) << "f" << std::endl;

	std::cout << "double: ";
	if (isIntegral(str))
		std::cout << static_cast<long>(str) << ".0" << std::endl;
	else
		std::cout << str << std::endl;
}

void printFloat(const std::string& input)
{	
	int min = std::numeric_limits<int>::min();
	int max = std::numeric_limits<int>::max();
	double dmin = -std::numeric_limits<double>::max();
	double dmax = std::numeric_limits<double>::max();
	const double fmax = std::numeric_limits<float>::max();
	errno = 0;
	std::string core = input.substr(0, input.size() - 1);
	double str = std::strtod(core.c_str(),NULL);

	if (errno == ERANGE)
	{
		printPseudoFromValue(str);
		return;
	}
	if(str != str || str > dmax || str < dmin)
	{
		printPseudoFromValue(str);
		return;
	}

	if (str < -fmax || str > fmax)
	{
		std::cout << "char: impossible" << std::endl;
		std::cout << "int: impossible" << std::endl;
		std::cout << "float: " << (str < 0 ? "-inff" : "+inff") << std::endl;
		std::cout << "double: ";
		if (isIntegral(str))
			std::cout << static_cast<long>(str) << ".0" << std::endl;
		else
			std::cout << str << std::endl;
		return;
	}

	if (str < 0.0f || str > 127.0f)
		std::cout << "char: impossible" << std::endl;
	else
	{
		unsigned char c = static_cast<unsigned char>(str);
		if (!std::isprint(c))
			std::cout << "char: Non displayable" << std::endl;
		else
			std::cout << "char: '" << static_cast<char>(c) << "'" << std::endl;
	}

	if (str < static_cast<double>(min) || str > static_cast<double>(max))
		std::cout << "int: impossible" << std::endl;
	else
		std::cout << "int: " << static_cast<int>(str) << std::endl;
	
	std::cout << "float: ";
	if (isIntegral(str))
		std::cout << static_cast<long>(str) << ".0f" << std::endl;
	else
		std::cout << static_cast<float>(str) << "f" << std::endl;

	std::cout << "double: ";
	if (isIntegral(str))
		std::cout << static_cast<long>(str) << ".0" << std::endl;
	else
		std::cout << str << std::endl;
}

void printInt(const std::string& input)
{
	int min = std::numeric_limits<int>::min();
	int max = std::numeric_limits<int>::max();
	errno = 0;
	long str = std::strtol(input.c_str(),NULL, 10);

	if (errno == ERANGE)
	{
		printDouble(input);
		return;
	}
	if (str < static_cast<long>(min) || str > static_cast<long>(max))
	{
		printDouble(input);
		return;
	}
	if (str < 0 || str > 127)
		std::cout << "char: impossible" << std::endl;
	else
	{
		unsigned char c = static_cast<unsigned char>(str);
		if (!std::isprint(c))
			std::cout << "char: Non displayable" << std::endl;
		else
			std::cout << "char: '" << static_cast<char>(c)<< "'" << std::endl;
	}
	std::cout << "int: " << static_cast<int>(str) << std::endl;

	std::cout << "float: ";
	if (isIntegral(str))
		std::cout << static_cast<long>(str) << ".0f" << std::endl;
	else
		std::cout << static_cast<float>(str) << "f" << std::endl;

	std::cout << "double: ";
	if (isIntegral(str))
		std::cout << static_cast<long>(str) << ".0" << std::endl;
	else
		std::cout << str << std::endl;
}


void ScalarConverter::convert(const std::string& input)
{
	int type = DetectType(input);
	switch (type)
	{
		case 0:
		case 1:
			printChar(input, type);
			break;
		case 2:
			printInt(input);
			break;
		case 3:
			printFloat(input);
			break;
		case 4:
			printDouble(input);
			break;
		case 5:
			printPseudo(input);
			break;
		case 6:
			throw invalidInput();
	}
}

