import os
import sys

if __name__ == "__main__":
  for (root, dirs, files) in os.walk('.'):
    for filename in files:
      filepath = os.path.join(root, filename)
      if filepath.endswith(".cpp") or filepath.endswith(".hpp"):
        with open(filepath) as f:
          s = f.read()
        if len(sys.argv) >= 2 and sys.argv[1] == 'i':
          s = s.replace("experimental::filesystem", "filesystem")
          s = s.replace("<experimental/filesystem>", "<filesystem>")
        else:
          s = s.replace("std::filesystem", "std::experimental::filesystem")
          s = s.replace("<filesystem>", "<experimental/filesystem>")
        with open(filepath, "w") as f:
          f.write(s)

  print("Done")
