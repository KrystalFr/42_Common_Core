/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Form.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 14:41:40 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 14:45:58 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FORM_HPP
# define FORM_HPP

# include <iostream>
# include <string>

class Bureaucrat;
class Form
{
	private:
		const std::string	name;
		const int			signGrade;
		const int			execGrade;
		bool				state;

	public:
		Form();
		Form(const std::string& name, const int signGrade, const int execGrade);
		Form(const Form& other);
		Form& operator=(const Form& other);
		~Form();

		std::string getName() const;
		int getsGrade() const;
		int geteGrade() const;
		bool isSigned() const;

		bool beSigned(const Bureaucrat& b);

		class GradeTooHighException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		class GradeTooLowException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
};

std::ostream& operator<<(std::ostream& os, const Form& b);
#endif