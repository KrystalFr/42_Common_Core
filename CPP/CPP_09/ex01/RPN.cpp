/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:19:26 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/27 14:51:56 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RPN.hpp"

bool is_op(char c)
{
	std::string op = "+-/*";
	for (int i = 0; i < 4; ++i)
	{
		if (c == op[i])
			return true;
	}
	return false;
}

int calculate(char op, int left, int right)
{
	int res = 0;

	if (op == '+')
		res = left + right;
	else if (op == '-')
		res = left - right;
	else if (op == '*')
		res = left * right;
	else if (op == '/')
	{
		res = left / right;
	}
	
	return res;
}

bool is_valid_input(const std::string &input)
{
	if (input.empty())
		return false;
	if (!std::isdigit(input[0]) || !is_op(input[input.size() - 1]))
		return false;
	for (size_t i = 0; i < input.size(); ++i)
	{
		if (input[i] != ' ')
		{
			if(i + 1 < input.size() && input[i + 1] != ' ')
				return false;
			if (!std::isdigit(input[i]) && !is_op(input[i]))
				return false;
		}
	}
	return true;
}

int RPN(const std::string &input)
{
	if (!is_valid_input(input))
		throw std::runtime_error("invalid token");
	
	std::stack<int, std::list<int> > st;
	for (size_t i = 0; i < input.size(); ++i)
	{
		if (std::isdigit(input[i]))
			st.push(input[i] - '0');
			
		if(is_op(input[i]))
		{
			if (st.size() < 2)
    			throw std::runtime_error("bad expression");
			int right = st.top();
			st.pop();
			int left = st.top();
			st.pop();
			st.push(calculate(input[i], left, right));
		}
	}
	if (st.size() > 1)
		throw std::runtime_error("bad expression");
	int res = st.top();
	return res;
}