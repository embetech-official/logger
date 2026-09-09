#include <embetech/logger.h>
#include <fstream>
#include <gtest/gtest.h>
#include <string>

namespace {

std::string ReadVersionFile() {
  std::ifstream ifs(std::string(PROJECT_DIR) + "/VERSION.txt");
  std::string content;
  if(!ifs.good()) {
    return {};
  }
  std::getline(ifs, content);
  return content;
}

std::string StripCommitId(std::string const &versionStr) {
  auto pos = versionStr.find('+');
  if(pos == std::string::npos) {
    return versionStr;
  }
  return versionStr.substr(0, pos);
}

} // namespace

TEST(LoggerVersion, MatchesVersionFileIgnoringCommitId) {
  std::string const rawVersion = ReadVersionFile();
  ASSERT_FALSE(rawVersion.empty()) << "Failed to read VERSION file";
  std::string const versionNoCommit = StripCommitId(rawVersion);

  char const *libStr = LOGGER_GetVersionString();
  ASSERT_NE(libStr, nullptr);
  EXPECT_EQ(StripCommitId(libStr), versionNoCommit);
}
