/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RPN.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/26 16:19:26 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/26 20:27:11 by krfranco         ###   ########.fr       */
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
	else if (op == '/')
		res = left / right;
	else if (op == '*')
		res = left * right;
	
	return res;
}

bool is_nb(char c)
{
	if (c <= '9' && c >= '0')
		return true;
	return false;
}

int RPN(std::string &input)
{
	std::stack<int, std::list<int> > st;

	for (int i = 0; i < input.size(); ++i)
	{
		if(is_nb(input[i]))
		{
			st.push(input[i] - '0');
		}

		if(is_op(input[i]))
		{
			if (st.size() < 2)
    			throw;
			int right = st.top();
			st.pop();
			int left = st.top();
			st.pop();
			st.push(calculate(input[i], left, right));
		}
	}
}