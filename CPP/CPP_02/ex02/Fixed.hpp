/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Fixed.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: krfranco <krfranco@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/18 23:28:55 by krfranco          #+#    #+#             */
/*   Updated: 2025/10/29 13:56:43 by krfranco         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FIXED_HPP
# define FIXED_HPP

# include <iostream>
# include <cmath>

class Fixed
{
	private:
		int					rawBits;
		static const int	fractBits;
	public:
    	Fixed(void);
		Fixed(int nb);
		Fixed(float f);
    	Fixed(const Fixed& other);
   		Fixed& operator=(const Fixed& other);
    	~Fixed(void);

		int getRawBits(void) const;
		void setRawBits(int const raw);
		
		float toFloat(void) const;
		int toInt( void ) const;

		bool operator>(const Fixed& other) const;
    	bool operator<(const Fixed& other) const;
   		bool operator>=(const Fixed& other) const;
    	bool operator<=(const Fixed& other) const;
    	bool operator==(const Fixed& other) const;
    	bool operator!=(const Fixed& other) const;

		Fixed operator+(const Fixed& other) const;
    	Fixed operator-(const Fixed& other) const;
    	Fixed operator*(const Fixed& other) const;
    	Fixed operator/(const Fixed& other) const;

		Fixed& operator++(void);
    	Fixed operator++(int);
    	Fixed& operator--(void);
    	Fixed operator--(int);

		static Fixed& min(Fixed& a, Fixed& b);
		static const Fixed& min(const Fixed& a, const Fixed& b);

		static Fixed& max(Fixed& a, Fixed& b);
		static const Fixed& max(const Fixed& a, const Fixed& b);
};

std::ostream& operator<<(std::ostream& os, const Fixed& fb);
#endif
