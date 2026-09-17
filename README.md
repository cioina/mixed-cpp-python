# mixed-cpp-python

## spam.vcxproj

```xml
 <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <AdditionalIncludeDirectories>C:\Python314\Include</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <AdditionalLibraryDirectories>C:\Python314\libs\;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <AdditionalIncludeDirectories>C:\Python314\Include</AdditionalIncludeDirectories>
    </ClCompile>
    <Link>
      <AdditionalLibraryDirectories>C:\Python314\libs\;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
    </Link>
  </ItemDefinitionGroup>
```
