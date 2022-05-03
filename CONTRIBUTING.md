# Code of Conduct
First of all, we would like to thank you for your decision towards contributing to this project. Kindly read through the [code of conduct](https://gitlab.com/autowarefoundation/autoware.auto/AutowareAuto/-/blob/master/CODE_OF_CONDUCT.md) adopted by the Autoware community to keep the engagements approachable and respectable. 

# License 
```AutoStreamForAutoware``` converter is licensed under Apache 2, and thus all contributions will be licensed as such
as per clause 9 of the [Apache 2 License](LICENSE.md):

```
9. Submission of Contributions. Unless You explicitly state otherwise, any Contribution intentionally submitted for inclusion in the Work by You to the Licensor shall be under the terms and conditions of this License, without any additional terms or conditions. Notwithstanding the above, nothing herein shall supersede or modify the terms of any separate license agreement you may have executed with Licensor regarding such Contributions.
```

# How to contribute

Contribution to this codebase can be done in three scenarios:
- If you encounter a problem, you can create an issue [here](https://github.com/tomtom-international/AutoStreamForAutoware/issues).
- You can address an existing issue by searching in the [list](https://github.com/tomtom-international/AutoStreamForAutoware/issues) of issues, and solve the problem following the coding guidelines and create a PR. 
- If the issue you want to solve is not listed as an existing issue, you must first create an issue and then solve it by following the coding guidelines and create a PR.

## Create a new issue
If you encounter a problem, we would like to suggest to use the search functionality to determine whether the issue already exists [here](https://github.com/tomtom-international/AutoStreamForAutoware/issues). If you can't find the issue related to your problem, you can create a new issue.

## Solve an issue
If you would like to solve an existing issue or a new issue which you have just created, you could scan through the list of open issue(s). Select the issue that you would like to solve and open a PR.

## Branch creating and commit message format. 
- If you are creating a branch for an issue to be resolved, make sure you follow the following format: ```<issue-id>-<branch name containing issue title>-<separated by>-<"-">``` for branch creation. For example, 
   - If the issue number is #1 with a title _Update LICENSE.md with updated copyright statements_, the branch name could be `1-update-license-md-with-updated-copyright-statements`.

- Once you have created your branch and developed code, the commit message must carry the following format, 
```
1. Add a title (first line) to your commit with the following characteristics:
   - It should complete the sentence: "If applied, this commit will ..."
   - The first letter should be capitalized
   - Do not exceed 50 characters to avoid short-log breakage
   - Do not end the subject line with a period
   - E.g. To convey the summary:
     "If applied, this commit will add precondition checks to the from input field"  
     -- Use the title: "Add precondition checks to the from input field"
 
2. Leave the next line empty to separate the title from the description.
 
3. Describe why the change is necessary.
 
4. Describe how the change addresses the issue.
 
5. Describe any concerns or potential side-effects of this change.
 
6. Tag the commit with the associated issue name/number:
   - E.g. To indicate that the commit fixes:
             ISSUE-1
          Add the line:
             "Fixes: [ISSUE-1]"
```

## Coding style

### Identifiers / Naming

### General:

- Type names and namespace names use _PascalCase_; variable, constant, and method names use _camelCase_.
- No underscores are used in the names.

### Name prefixes:

- All class and struct names shall be named starting with upper-case C. For example, CSomeClass.
- All typedefs and enumerator type names shall be named starting with upper-case T. For example, TSomeTypeAlias.
- All method parameters shall be named starting with lower-case a. For example, aArgument, aParameter.
- All template parameters (type, non-type, and template) shall be named starting with lower case ta. For example, taType, taValue.
- All class data members shall be named starting with lower-case m. For example, mIsInitialized.
- All constant values shall be named starting with lower-case k. For example, kDefaultHttpPort.
- All enumerator values shall be named starting with lower-case k and the name of the enum type (not including the T prefix). For example, kStatusOk and kStatusError in an enum named TStatus.
- Namespaces shall not have prefix.

### Namespaces:

- All component code shall be in namespace ```TomTom::AutoStreamForAutoware```
- Additional inner namespaces may be used where appropriate.
- Use of ```using namespace``` is not allowed, full namespaces must be used unless the referred entity (class, struct, etc)  is part of the current namespace.

### Include style
- All includes of the code shall be using double-quotes: #include "MyHeader.hpp"
- All includes of third party headers shall be using pointy brackets: #include <sqlite3.h>
- Order of includes: 
    - own header file (the own .hpp should be included first in a .cpp)
    - Empty line 
    - Other own-code header files
    - Empty line
    - Third-party header files; standard header files

### File naming

- Files shall be named after the (main) class defined in the file, without the C prefix. For example, MyClass.hpp, defining class CMyClass.

- Implementation files shall have extension .cpp.

### Include guards

- Include guards are all capitals and of the form: ```TOMTOM_<APPLICATION>_<COMPONENT_NAME>_<FILENAME_WITHOUT_EXTENSION>_H```. E.g., ```Component/AutoStreamMapConverter/include/AutoStreamMapConverter/ArcConverter.hpp``` shall have an include guard named ```TOMTOM_AUTOSTREAM_MAP_CONVERTER_ARC_CONVERTER_H```.

- For other headers, similar pattern should be followed to make sure the include guards are unique. 

- The closing #endif should not have a comment repeating the include guard name.

## Doxygen comments
All the code written should include Doxygen comments. Upon releasing the software, the Doxygen documents are generated. At the code-level, a developer can use the generated documents to integrate and use the code by reading each function or class description and understanding the input or output parameters' definition. 

A problem with the Doxygen style comments is the need for continuous maintenance of these documents to avoid them getting out-dated. At every change, 

- Verify the parameters or functions are still correctly documented,
- Avoid using any parameter name or function/class name within the documents as tracking the change of these names within the document is complicated.

# Release management
The version of the release will be incremented based on minor and major releases.

Minor release: V1.x
- Any minor upgrade to existing features will be considered as minor.
- Example: V1.1, V1.2 etc.

Major release: Vx.0
- Any major upgrade like adding a new feature will be considered as major.
- Example: V2.0, V3.0 etc.