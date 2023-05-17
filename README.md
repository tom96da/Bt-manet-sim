# Bt-manet-sim
Bluetooth MANET simulation with C++  
Bluetooth MANET を C++ でシミュレーションする。

## 環境
各ローカルリポジトリの環境を以下に示す。
|         |  rigil   | AINCRAD  |
| ------- | :------: | :------: |
|  C++    |    20    |    20    |
|  g++    |  12.1.0  |  12.2.0  |
|  CMake  |   3.22   |   3.22   |
|  Git    |  2.34.1  |  2.40.0  |



## ワークスペース設定事項
実行ファイルが Git に追跡されないよう、VScode のワークスペースは以下のようにの設定する。

```
{
    "settings": {
        "c-cpp-compile-run.output-location": "out",
        "code-runner.executorMap": {            
            "cpp": "cd $dir && g++ $fileName -o out/$fileNameWithoutExt && out/$fileNameWithoutExt"
        }
    }
}
```

また、`.gitignore`は以下のように設定する。

```
# output-file
*.out
```


拡張機能を用いてコンパイルおよび実行するときの、実行ファイルの出力先を `out/` に指定し、  
実行ファイルの拡張子を指定し Git の追跡対象外としている。  
~~のちで設定する CMake によって生成される `build/` 以下のファイルは自動的に追跡対象外になる模様。~~

### 追記（2023.5.17）
以前より `.gitignore` 自身を追跡対象に含めたため、リポジトリをクローンした際に新たに作成する必要はなくなった。

## CMake の設定事項
CMakeは分割されたソースファイルからビルドを自動で行ってくれる。

1. CMake のインストール  
    Linux mint の場合
    ```
    $ apt install cmake
    ```

2. 拡張機能のインストール  
    VScode の拡張機能 `CMake Tools` が必要となる。

3. CMakeLists.txt の作成  
    プログラムファイルと同じディレクトリに `CMakeLists.txt` を作成する。  
    Git の追跡対象に含めるため、クローンした場合は作成不要。
    ```
    cmake_minimum_required(VERSION 3.22)  # CMake version check
    project(Bt_manet_sim)                 # Create project "simple_example"
    set(CMAKE_CXX_STANDARD 20)            # Enable c++20 standard

    # Add main.cpp file of project root directory as source file
    set(SOURCE main.cpp)

    # Add executable target with source files listed in SOURCE_FILES variable
    add_executable(Bt_manet_sim ${SOURCE})
    ```
    分割されたソースファイルを `set(SOURCE main.cpp hege.cpp foo.cpp)` と羅列する。

4. コマンドパレットから、CMakeクイックスタートを実行する  
    このとき、エディタは `main.cpp` にカーソルがある状態にしておくこと。  
    参照する `CMakeLists.txt` を提示されるので、作成したものを選択する。  
    使用するコンパイラなどを選択し、プロジェクト作成を進める。
    このプロジェクトでは、`GCC12.x` を選択する。  
    `CMakeLists.txt` のパスを選択とプロジェクト名の名称を入力すれば、作成が終わる。

5. ビルドを実行する
    ステータスバーからビルドや実行ができるようになる。
    キーボードショートカットもあるため要確認。

## CMake にともなう追加の設定
`.gitignore` にCMakeによって生成されるファイルの記述を追加する。
```
# CMake-generated files
*.cmake
CMakeFiles
Testing
CMakeCache.txt
compile_commands.json
Makefile

# CMake project
Bt_manet_sim　　

# local setting
.vscode
```

~~追跡対象とするファイルを以下に示す。~~
- ~~CMakeLists.txt：プロジェクトのビルド手順を記述するためのファイル~~
- ~~.cmakeファイル：CMakeLists.txtファイルで使用されるサブモジュールのようなファイル~~
- ~~.inファイル：ビルドプロセス中に変数を埋め込むためのテンプレートファイル~~
