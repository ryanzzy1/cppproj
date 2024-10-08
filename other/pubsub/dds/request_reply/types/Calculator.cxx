// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file Calculator.cpp
 * This source file contains the implementation of the described types in the IDL file.
 *
 * This file was generated by the tool fastddsgen.
 */

#ifdef _WIN32
// Remove linker warning LNK4221 on Visual Studio
namespace {
char dummy;
}  // namespace
#endif  // _WIN32

#include "Calculator.h"

#include <fastcdr/Cdr.h>


#include <fastcdr/exceptions/BadParamException.h>
using namespace eprosima::fastcdr::exception;

#include <utility>




CalculatorRequestType::CalculatorRequestType()
{
}

CalculatorRequestType::~CalculatorRequestType()
{
}

CalculatorRequestType::CalculatorRequestType(
        const CalculatorRequestType& x)
{
    m_client_id = x.m_client_id;
    m_operation = x.m_operation;
    m_x = x.m_x;
    m_y = x.m_y;
}

CalculatorRequestType::CalculatorRequestType(
        CalculatorRequestType&& x) noexcept
{
    m_client_id = std::move(x.m_client_id);
    m_operation = x.m_operation;
    m_x = x.m_x;
    m_y = x.m_y;
}

CalculatorRequestType& CalculatorRequestType::operator =(
        const CalculatorRequestType& x)
{

    m_client_id = x.m_client_id;
    m_operation = x.m_operation;
    m_x = x.m_x;
    m_y = x.m_y;
    return *this;
}

CalculatorRequestType& CalculatorRequestType::operator =(
        CalculatorRequestType&& x) noexcept
{

    m_client_id = std::move(x.m_client_id);
    m_operation = x.m_operation;
    m_x = x.m_x;
    m_y = x.m_y;
    return *this;
}

bool CalculatorRequestType::operator ==(
        const CalculatorRequestType& x) const
{
    return (m_client_id == x.m_client_id &&
           m_operation == x.m_operation &&
           m_x == x.m_x &&
           m_y == x.m_y);
}

bool CalculatorRequestType::operator !=(
        const CalculatorRequestType& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member client_id
 * @param _client_id New value to be copied in member client_id
 */
void CalculatorRequestType::client_id(
        const std::string& _client_id)
{
    m_client_id = _client_id;
}

/*!
 * @brief This function moves the value in member client_id
 * @param _client_id New value to be moved in member client_id
 */
void CalculatorRequestType::client_id(
        std::string&& _client_id)
{
    m_client_id = std::move(_client_id);
}

/*!
 * @brief This function returns a constant reference to member client_id
 * @return Constant reference to member client_id
 */
const std::string& CalculatorRequestType::client_id() const
{
    return m_client_id;
}

/*!
 * @brief This function returns a reference to member client_id
 * @return Reference to member client_id
 */
std::string& CalculatorRequestType::client_id()
{
    return m_client_id;
}


/*!
 * @brief This function sets a value in member operation
 * @param _operation New value for member operation
 */
void CalculatorRequestType::operation(
        CalculatorOperationType _operation)
{
    m_operation = _operation;
}

/*!
 * @brief This function returns the value of member operation
 * @return Value of member operation
 */
CalculatorOperationType CalculatorRequestType::operation() const
{
    return m_operation;
}

/*!
 * @brief This function returns a reference to member operation
 * @return Reference to member operation
 */
CalculatorOperationType& CalculatorRequestType::operation()
{
    return m_operation;
}


/*!
 * @brief This function sets a value in member x
 * @param _x New value for member x
 */
void CalculatorRequestType::x(
        int16_t _x)
{
    m_x = _x;
}

/*!
 * @brief This function returns the value of member x
 * @return Value of member x
 */
int16_t CalculatorRequestType::x() const
{
    return m_x;
}

/*!
 * @brief This function returns a reference to member x
 * @return Reference to member x
 */
int16_t& CalculatorRequestType::x()
{
    return m_x;
}


/*!
 * @brief This function sets a value in member y
 * @param _y New value for member y
 */
void CalculatorRequestType::y(
        int16_t _y)
{
    m_y = _y;
}

/*!
 * @brief This function returns the value of member y
 * @return Value of member y
 */
int16_t CalculatorRequestType::y() const
{
    return m_y;
}

/*!
 * @brief This function returns a reference to member y
 * @return Reference to member y
 */
int16_t& CalculatorRequestType::y()
{
    return m_y;
}




CalculatorReplyType::CalculatorReplyType()
{
}

CalculatorReplyType::~CalculatorReplyType()
{
}

CalculatorReplyType::CalculatorReplyType(
        const CalculatorReplyType& x)
{
    m_client_id = x.m_client_id;
    m_result = x.m_result;
}

CalculatorReplyType::CalculatorReplyType(
        CalculatorReplyType&& x) noexcept
{
    m_client_id = std::move(x.m_client_id);
    m_result = x.m_result;
}

CalculatorReplyType& CalculatorReplyType::operator =(
        const CalculatorReplyType& x)
{

    m_client_id = x.m_client_id;
    m_result = x.m_result;
    return *this;
}

CalculatorReplyType& CalculatorReplyType::operator =(
        CalculatorReplyType&& x) noexcept
{

    m_client_id = std::move(x.m_client_id);
    m_result = x.m_result;
    return *this;
}

bool CalculatorReplyType::operator ==(
        const CalculatorReplyType& x) const
{
    return (m_client_id == x.m_client_id &&
           m_result == x.m_result);
}

bool CalculatorReplyType::operator !=(
        const CalculatorReplyType& x) const
{
    return !(*this == x);
}

/*!
 * @brief This function copies the value in member client_id
 * @param _client_id New value to be copied in member client_id
 */
void CalculatorReplyType::client_id(
        const std::string& _client_id)
{
    m_client_id = _client_id;
}

/*!
 * @brief This function moves the value in member client_id
 * @param _client_id New value to be moved in member client_id
 */
void CalculatorReplyType::client_id(
        std::string&& _client_id)
{
    m_client_id = std::move(_client_id);
}

/*!
 * @brief This function returns a constant reference to member client_id
 * @return Constant reference to member client_id
 */
const std::string& CalculatorReplyType::client_id() const
{
    return m_client_id;
}

/*!
 * @brief This function returns a reference to member client_id
 * @return Reference to member client_id
 */
std::string& CalculatorReplyType::client_id()
{
    return m_client_id;
}


/*!
 * @brief This function sets a value in member result
 * @param _result New value for member result
 */
void CalculatorReplyType::result(
        int32_t _result)
{
    m_result = _result;
}

/*!
 * @brief This function returns the value of member result
 * @return Value of member result
 */
int32_t CalculatorReplyType::result() const
{
    return m_result;
}

/*!
 * @brief This function returns a reference to member result
 * @return Reference to member result
 */
int32_t& CalculatorReplyType::result()
{
    return m_result;
}


// Include auxiliary functions like for serializing/deserializing.
#include "CalculatorCdrAux.ipp"

