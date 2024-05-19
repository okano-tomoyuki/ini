# Ini

Iniはiniファイルを取り扱うためのパーサライブラリとして開発されました。

## 1. 準備

DataTimeはC++11以降の環境で動作するシングルヘッダーのライブラリとして
開発してされています。(依存ライブラリ等はなく、includeするだけで使えます。)

## 2. 使い方

### 2.1 読込

コンストラクタで対象のiniファイルパスを指定し、セクション名とフィールド名を指定し
値を取得します。取得時の型はbool/int/double/std::stringに対応しています。
以下の場合には第3引数で渡したデフォルト値を返却します。

- 対象ファイルが存在しない
- 一致するセクション・フィールドが存在しない
- 対象フィールドを指定の型として取得できない。

``` cpp
// hoge.ini
// 
// [SECTION1]
// FIELD1 = 10.5
// FIELD2 = hello world
// 
// [SECTION2]
// FIELD1 = TRUE
// FIELD2 = 1

auto ini     = Ini("hoge.ini");
auto d_value = ini.read_double("SECTION1", "FIELD1", -999.9); // 10.5
auto nothing = ini.read_double("SECTION1", "FIELDX", -999.9); // -999.9 
auto s_value = ini.read_str("SECTION1", "FIELD2", "nothing"); // "hello world"
auto b_value = ini.read_bool("SECTION2", "FIELD1", false);    // true
```

### 2.2 書込

書込時同様コンストラクタで対象のiniファイルパスを指定し、セクション名とフィールド名を指定し値を書き込みます。対象セクションとフィールドが存在しない場合は追加され、存在している場合は上書きされます。書込成功・失敗をtrue/falseで返却します。

``` cpp
// hoge.ini
// 
// [SECTION1]
// FIELD1 = 10.5
// FIELD2 = hello world
// 
// [SECTION2]
// FIELD1 = TRUE

auto ini     = Ini("hoge.ini");
ini.write_double("SECTION1", "FIELD1", -999.9);
ini.write_double("SECTION1", "FIELDX", -999.9); 
ini.write_str("SECTION1", "FIELD2", "nothing");
ini.write_bool("SECTION2", "FIELD1", false);
ini.write_int("SECTIONX", "FIELD1", 10);

// hoge.ini
// 
// [SECTION1]
// FIELD1 = -999.9
// FIELD2 = nothing
// FIELDX = -999.9
// [SECTION2]
// FIELD1 = FALSE
// [SECTIONX]
// FIELD1 = 10
```

## 3. 利用上の注意点

インスタンス生成時に対象となるiniファイルをオープンするのではなく、
各read/writeメソッド呼び出し時にその都度ファイルをオープンしているため
iniファイルが大規模の場合の使用には向いていません。