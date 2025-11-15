#include <gtest/gtest.h>
#include "utils.hpp"
#include <filesystem>
#include <fstream>

TEST(UtilsLanguageTest, DifferentLanguages) {
    EXPECT_EQ(getLanguageExtension(".cpp"), "cpp");
    EXPECT_EQ(getLanguageExtension(".h"), "cpp");
    EXPECT_EQ(getLanguageExtension(".js"), "javascript");
    EXPECT_EQ(getLanguageExtension(".md"), "markdown");
    EXPECT_NE(getLanguageExtension(".unknown"), "cpp");
    EXPECT_EQ(getLanguageExtension(""), "text");  // Unknown returns "text"
}

TEST(UtilsMatchingTest, WildCardMatch) {
    EXPECT_TRUE(matchingFileDir("fakeFile.txt", "*"));
    EXPECT_FALSE(matchingFileDir("file.txt", ""));

    auto path = (std::filesystem::path("path") / "build" / "output").string();
    EXPECT_TRUE(matchingFileDir(path, "*build"));
    EXPECT_TRUE(matchingFileDir("main.cpp", "*.cpp"));
    EXPECT_TRUE(matchingFileDir("unittest", "test"));
    EXPECT_FALSE(matchingFileDir("", "**"));
}

TEST(UtilsExcludedTest, ExcludedExtensions) {
    auto tempFile = std::filesystem::temp_directory_path() / "test.o";
    std::ofstream(tempFile).close();

    EXPECT_TRUE(std::filesystem::exists(tempFile));

    EXPECT_TRUE(excludedExtensions(tempFile.string(), "*.o"));
    EXPECT_FALSE(excludedExtensions(tempFile.string(), "*.cpp"));
    EXPECT_TRUE(excludedExtensions(tempFile.string(), "*.o, *.a, *.lib"));
    EXPECT_FALSE(excludedExtensions(tempFile.string(), "*.cpp, *.h"));

    std::filesystem::remove(tempFile);

    EXPECT_FALSE(std::filesystem::exists(tempFile));
}

TEST(UtilsIncludeTest, OnlyIncludedExtensions) {
    auto tempFile = std::filesystem::temp_directory_path() / "include_test.cpp";
    {
        std::ofstream temp(tempFile);
        temp << "int main() { return 0; }\n";
    }

    EXPECT_TRUE(onlyIncludedExtensions(tempFile.string(), "*.cpp"));
    EXPECT_TRUE(onlyIncludedExtensions(tempFile.string(), "*.hpp, *.cpp"));
    EXPECT_TRUE(onlyIncludedExtensions(tempFile.string(), "  *.cpp  "));
    EXPECT_FALSE(onlyIncludedExtensions(tempFile.string(), "*.py, *.md"));
    EXPECT_TRUE(onlyIncludedExtensions(tempFile.string(), ""));

    std::filesystem::remove(tempFile);
}
