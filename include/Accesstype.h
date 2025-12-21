/**
 * @file Accesstype.h
 * @brief Header file defining access types for shared variable operations
 */

#ifndef ACCESS_TYPE_H
#define ACCESS_TYPE_H

/**
 * @enum AccessType
 * @brief Types of access operations on shared variables
 */
enum class AccessType {
    READ,   ///< Read access operation
    WRITE   ///< Write access operation
};

#endif // ACCESS_TYPE_H