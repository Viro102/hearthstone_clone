#include <Common.h>

namespace Common {
    vector<int> extractNumbers(const string &input) {
        vector<int> numbers;
        size_t startPos = 0;
        size_t endPos = 0;

        while (true) {
            startPos = input.find('[', startPos);
            endPos = input.find(']', startPos);

            if (startPos == string::npos || endPos == string::npos) {
                break;
            }

            string numberStr = input.substr(startPos + 1, endPos - startPos - 1);

            numbers.push_back(std::stoi(numberStr));

            startPos = endPos + 1;
        }

        return numbers;
    }
}
