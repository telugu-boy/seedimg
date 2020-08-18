#ifndef IRDUMP_H
#define IRDUMP_H

#include <seedimg/seedimg.hpp>
#include <filesystem>

namespace seedimg::modules {
/**
 * Seedimg IR dump format is a custom seedimg-specific format,
 * which is CPU-endianness agnostic made to represent exact
 * core-contents of the IR for debugging and interchange.
 *
 * NOTE: there's no signature of this format, thus no way to
 * predict its identity, currently only this library does
 * support this format.
 */
namespace irdump {
  /**
   * @brief Encode a given image in the Seedimg IR dump format to a filepath.
   * @param filepath a valid filepath (with/wihout filextension).
   * @param input the given image to encode.
   * @return true on success, false on failure.
   */
  bool to(const std::filesystem::path          filepath,
          const std::unique_ptr<seedimg::img>& input);

  /**
   * @brief Decode a image in the Seedimg IR dump format from a filepath.
   * @param filepath a valid filepath (with/without filextension).
   * @return a non-null image on success, null on failure.
   */
  std::unique_ptr<seedimg::img>
  from(const std::filesystem::path filepath);
}
}


#endif // IRDUMP_H
