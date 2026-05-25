/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Form.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 14:41:31 by krfranco          #+#    #+#             */
/*   Updated: 2026/05/25 16:28:37 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Form.hpp"
#include "../includes/Bureaucrat.hpp"


Form::Form():name("default"), signGrade(150), execGrade(150)
{
    this->state = false;
}

Form::Form(const std::string& n, const int sg, const int eg)
	: name(n)
	, signGrade(sg)
	, execGrade(eg)
	, state(false)
{
	std::cout << "Form: Constructor called" << std::endl;
	if (signGrade < 1 || execGrade < 1)
		throw GradeTooHighException();
	if (signGrade > 150 || execGrade > 150)
		throw GradeTooLowException();
}

Form::Form(const Form& other)
    : name(other.name)
    , signGrade(other.signGrade)
    , execGrade(other.execGrade)
    , state(other.state)
{
	std::cout << "Form: Copy constructor called" << std::endl;
}

Form& Form::operator=(const Form& other)
{
	std::cout << "Form: Copy assignement operator called" << std::endl;
	if (this != &other)
		this->state = other.state;
	return *this;
}

Form::~Form()
{
	std::cout << "Form: Destructor called" << std::endl;
}

std::string Form::getName() const
{
	return this->name;
}

int Form::getsGrade() const
{
	return this->signGrade;
}

int Form::geteGrade() const
{
	return this->execGrade;
}

bool Form::isSigned() const
{
	return this->state;
}

bool Form::beSigned(const Bureaucrat& b)
{
	if (b.getGrade() > this->signGrade)
		throw GradeTooLowException();
	if (!this->state)
	{
		this->state = true;
		return true;
	}
	else
	{
		std::cout << b.getName() << " couldn’t sign " << this->name << " because: it's already signed" << std::endl;
		return false;
	}
}

const char* Form::GradeTooLowException::what() const throw(){
	return "Grade too low";
}

const char* Form::GradeTooHighException::what() const throw(){
	return "Grade too high";
}

std::ostream& operator<<(std::ostream& os, const Form& f)
{
	std::string state;
	if (f.isSigned())
		state = "signed";
	else
		state = "unsigned";

	os << f.getName() << " form is " << state << ".\n"
		<< "Grade of " << f.getsGrade() << " required to sign\n"
		<< "Grade of " << f.geteGrade() << " required to execute";
	return os;
}