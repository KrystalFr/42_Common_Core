#!/usr/bin/env bash
# gen_ocf.sh - Générateur simple de classes OCF (zsh/bash compatible)
# Usage: ./gen_ocf.sh Fixed Zombie MyClass

set -euo pipefail

if (( $# < 1 )); then
  echo "Usage: $0 ClassName [ClassName2 ...]"
  exit 1
fi

created=()

for cls in "$@"; do
  # Normaliser le nom : première lettre majuscule, reste inchangé
  # Portablement : on utilise awk/printf/tr
  first="$(printf '%s' "$cls" | awk '{print substr($0,1,1)}')"
  rest="$(printf '%s' "$cls" | awk '{print substr($0,2)}')"
  first_cap="$(printf '%s' "$first" | tr '[:lower:]' '[:upper:]')"
  capClass="${first_cap}${rest}"

  # Guard en majuscules, non-alphanum -> _
  fcapClass="$(printf '%s' "$capClass" | tr '[:lower:]' '[:upper:]' | sed 's/[^A-Z0-9]/_/g')"

  hpp="${capClass}.hpp"
  cpp="${capClass}.cpp"

  # Ecrire le header
  cat > "${hpp}" <<EOF
#ifndef ${fcapClass}_HPP
# define ${fcapClass}_HPP

# include <string>
# include <iostream>

class ${capClass}
{
	private:

	public:
    	${capClass}();
    	${capClass}(const ${capClass}& other);
   		${capClass}& operator=(const ${capClass}& other);
    	~${capClass}();
};

#endif
EOF

  # Ecrire le cpp
  cat > "${cpp}" <<EOF
#include "../includes/${capClass}.hpp"

${capClass}::${capClass}()
{
    std::cout << "${capClass}: Constructor called" << std::endl;
}

${capClass}::${capClass}(const ${capClass}& other)
{
    std::cout << "${capClass}: Copy constructor called" << std::endl;
	*this = other;
}

${capClass}& ${capClass}::operator=(const ${capClass}& other)
{
    std::cout << "${capClass}: Copy assignment operator called" << std::endl;
}

${capClass}::~${capClass}()
{
    std::cout << "${capClass}: Destructor called" << std::endl;
}
EOF

  created+=("${hpp}" "${cpp}")
  printf "Created: %s and %s\n" "${hpp}" "${cpp}"
done

echo
printf "%d files created:\n" "${#created[@]}"
for f in "${created[@]}"; do
  printf " - %s\n" "$f"
done