/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AForm.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 14:41:40 by krfranco          #+#    #+#             */
/*   Updated: 2026/03/03 14:37:58 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AFORM_HPP
# define AFORM_HPP

# include <iostream>
# include <string>

class Bureaucrat;
class AForm
{
	private:
		const std::string	name;
		const int			signGrade;
		const int			execGrade;
		bool				state;

	public:
		AForm();
		AForm(const std::string& name, const int signGrade, const int execGrade);
		AForm(const AForm& other);
		AForm& operator=(const AForm& other);
		virtual ~AForm() = 0;

		std::string getName() const;
		int getsGrade() const;
		int geteGrade() const;
		bool isSigned() const;

		bool beSigned(const Bureaucrat& b);
		void execute(Bureaucrat const & executor) const;

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
		
		class NotSignedException : public std::exception
		{
			public:
				virtual const char* what() const throw();
		};
		
	protected :
	   virtual void executeAction() const = 0;
};

std::ostream& operator<<(std::ostream& os, const AForm& b);
#endif