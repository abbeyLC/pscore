/**************************************************************************************************
*                                            PSCORE                                               *
*                               Copyright (C) 2020 Pointon Software                               *
*                                                                                                 *
*           This program is free software: you can redistribute it and/or modify                  *
*           it under the terms of the GNU Affero General Public License as published              *
*           by the Free Software Foundation, either version 3 of the License, or                  *
*           (at your option) any later version.                                                   *
*                                                                                                 *
*           This program is distributed in the hope that it will be useful,                       *
*           but WITHOUT ANY WARRANTY; without even the implied warranty of                        *
*           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                         *
*           GNU Affero General Public License for more details.                                   *
*                                                                                                 *
*           You should have received a copy of the GNU Affero General Public License              *
*           along with this program.  If not, see <https://www.gnu.org/licenses/>.                *
*                                                                                                 *
*           Ben Ziv <pointonsoftware@gmail.com>                                                   *
*                                                                                                 *
**************************************************************************************************/
#ifndef CORE_ENTITY_VALIDATOR_PERSONVALIDATOR_HPP_
#define CORE_ENTITY_VALIDATOR_PERSONVALIDATOR_HPP_
// Parent
#include "validator.hpp"
// Entity
#include <person.hpp>

namespace entity {
namespace validator {

class PersonValidator : public Validator {
 public:
    explicit PersonValidator(const Person& person);
    ~PersonValidator() = default;

 private:
    Person mPerson;
    // Validation functions
    void validateFirstName();
    void validateMiddleName();
    void validateLastName();
};

}  // namespace validator
}  // namespace entity
#endif  // CORE_ENTITY_VALIDATOR_PERSONVALIDATOR_HPP_