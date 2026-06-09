/**
 * @file version.h
 * @brief Version information for LibRift
 *
 * Provides version constants and macros for the LibRift library.
 *
 * @copyright OBINexus Computing, 2025
 */

#ifndef LIBRIFT_VERSION_H
#define LIBRIFT_VERSION_H

/**
 * @brief LibRift version information macros
 */
#define LIBRIFT_VERSION_MAJOR 1
#define LIBRIFT_VERSION_MINOR 0
#define LIBRIFT_VERSION_PATCH 0
#define LIBRIFT_VERSION_STRING "1.0.0"

/**
 * @brief Helper macro to create version integer
 */
#define LIBRIFT_MAKE_VERSION(major, minor, patch) (((major) << 16) | ((minor) << 8) | (patch))

/**
 * @brief Current version as an integer
 */
#define LIBRIFT_VERSION                                                                            \
    LIBRIFT_MAKE_VERSION(LIBRIFT_VERSION_MAJOR, LIBRIFT_VERSION_MINOR, LIBRIFT_VERSION_PATCH)

/**
 * @brief Check if current version is at least the specified version
 */
#define LIBRIFT_VERSION_CHECK(major, minor, patch)                                                 \
    (LIBRIFT_VERSION >= LIBRIFT_MAKE_VERSION(major, minor, patch))

#endif // LIBRIFT_VERSION_H