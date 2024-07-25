---
# Backtest

## Introduction

Backtest is a program to test trading strategies. It starts with a tick file of any instrument in csv format.

The input tick file must not contain any headers. From this file you can create a csv file for different timeframes. It is this timeframe file that is then used in the indicator calculations.

The timeframe file is then read into the system at the time of testing, all indicators are calculated as specified in a json file. The indicator json file is specified inthe main configuration file. From there, trade triggers and conditions are adhered to as defined in the trigger json file, also specified in the main config file. There are also exit conditions specified by the exits json file, again also in main config file.

After all this information is processed, the application then starts to run the rules using the tick file as input and produces a report on the results.

## Script files

In the json config files, the paradigm is to specify a user defined name, an input name, **Verb** and another input name. The verb is something like "above", "crossover" or "below". The normal relational verbs are built into the program. These are listed below.

| Verb | Description |
|------|-------------|
| AND  | Logical AND. If the first input is true AND the second input is true, then the result is also true. |
| OR   | Lohical OR. If either of the inputs are true, then the result is also true. |

Script files can extend the verbs. The system populates 3 global variables. ind1, ind2 and res. These are all tables (c++ std::vector). Your code should use ind1 and ind2 for calculations and put results in res.

Any verb in the config json files that contains a ":" specifies a script file to execute. For example, if you want to do an exclusive or, the config line would have a verb like this: ":xor" or "lib:xor".

Both means that a file called xor.lua is present in the script path as specified in the main config file. The portion before the ":" is the name of a lua library to be loaded, and the part after the ":" is the name of the function.

This sample shows the use of the variables:
<pre><code>
  for i = 1, #ind1 do
    if ind1[i] == 0 and ind2[i] == 0 then res[i] = 0
    elseif ind1[i] == 0 and ind2[i] == 1 then res[i] = 1
    elseif ind1[i] == 1 and ind2[i] == 0 then res[i] = 1
    elseif ind1[i] == 1 and ind2[i] == 1 then res[i] = 0
    end
  end
</code></pre>

## Features

- Generate any timeframe data from tick data
- Calculate any financial indicators
- Configure trade rules
- Apply the trade rules and test the outcome
- LUA scripts for easy custom indicators
- Generates reports of the results
- Easy to configure
- Time constraints
- Trade volume constraints
- Amount of trade constraints
- Day of the week constraints
- Enable or Disabled rules
- Buy or Sell or Both

## Configuration

### Main config file

The main config file is the starting point for any system. It is in the properties file format and has the following settings:

system.name=*Any string for the name of the trading system*
system.Description=*Description for the system*
system.path=The path to the system setup. E.g. */home/cv/Dev/cpp/trading/systems*
script.path=The path to lua scripts. E.g. */home/cv/Dev/cpp/trading/scripts*
indicators=The filename of the indicators to calculate. Located in the system path. E.g. *indicators.json*
triggers=The filename for the trade conditions and triggers. Located in the system path. E.g. *triggers.json*
exits=The filename for the exit conditions. E.g. *exits.json*
results=The filename and path for the report. E.g. *results.csv or ~/Reports/Trade.pdf*

### Indicators file

In all the json files where verbs are specified, if it starts with a ":", then it refers to a lua script file. More about scripts later.

The indicators file is in json format. It has a generic format for all indicators. It is pretty self explanatory and here is a sample file.
```json
{
  "Indicators": [
    {
      "indName": "EMA150",
      "name": "ema",
      "timeFrame": "30",
      "fullName": "Exponential Moving Average",
      "type": 1,
      "inputs": 1,
      "options": 1,
      "outputs": 1,
      "inputNames": [
        "close"
      ],
      "optionNames": [
        "Period"
      ],
      "optionValues": [
        150
      ],
      "outputNames": [
        "EMA150"
      ]
    }
  ]
}
```
### Triggers file

The trigger file is a json file located in the system path as specified in the main config file. The layout is self explanatory. An example follows:

```json
{
  "ENABLED": true,
  "BUY": [
    {
      "Name": "B1",
      "Ind1": "EMA150",
      "Verb": "crossup",
      "Ind2": "EMA230"
    },
    {
      "Name": "B2",
      "Ind1": "EMA150",
      "Verb": "above",
      "Ind2": "SMA20"
    },
    {
      "Name": "B3",
      "Ind1": "EMA230",
      "Verb": "above",
      "Ind2": "SMA20"
    },
    {
      "Name": "B4",
      "Ind1": "RSI",
      "Verb": "above",
      "Ind2": "#30"
    }
  ],
  "SELL": [
    {
      "Name": "S1",
      "Ind1": "EMA150",
      "Verb": "crossdown",
      "Ind2": "EMA230"
    },
    {
      "Name": "S2",
      "Ind1": "EMA150",
      "Verb": "below",
      "Ind2": "SMA20"
    },
    {
      "Name": "S3",
      "Ind1": "EMA230",
      "Verb": "below",
      "Ind2": "SMA20"
    }
  ],
  "BUY_OPERATIONS": [
    {
      "Name": "BUY1",
      "Ind1": "B1",
      "Verb": "AND",
      "Ind2": "B2"
    },
    {
      "Name": "BUY2",
      "Ind1": "B2",
      "Verb": "AND",
      "Ind2": "B3"
    },
    {
      "Name": "Xor",
      "Ind1": "B2",
      "Verb": "MyVerbs:XOR",
      "Ind2": "B3"
    }
  ],
  "SELL_OPERATIONS": [
    {
      "Name": "SELL1",
      "Ind1": "S1",
      "Verb": "AND",
      "Ind2": "S2"
    },
    {
      "Name": "SELL2",
      "Ind1": "S2",
      "Verb": "AND",
      "Ind2": "S3"
    },
    {
      "Name": "Sma",
      "Ind1": "B2",
      "Verb": ":sma",
      "Ind2": "B3"
    }
  ],
  "BUY_COLUMN": "BUY2",
  "SELL_COLUMN": "SELL2",
  "MAX_BUY": 1,
  "MAX_SELL": 1,
  "BUY_AND_SELL": "true",
  "TIMING": [
    {
      "START": "09H00",
      "END": "15H00",
      "DAYS": [
        1,
        2,
        3,
        4,
        5
      ]
    }
  ]
}
```

### Exits file

The Exit file is a json file located in the system path as specified in the main config file. It is used to define exit strategies. A self explanatory sample follows.

```json
{
  "BUY_EXIT": [
    {
      "Name": "E1",
      "Ind1": "BID",
      "Verb": "below",
      "Ind2": "SMA20"
    }
  ],
  "SELL_EXIT": [
    {
      "Name": "E2",
      "Ind1": "ASK",
      "Verb": "above",
      "Ind2": "SMA20"
    }
  ]
}
```

## Input Files

### Tick file

The tick file is a csv file that has the following columns:

`Date Time, Bid, Ask, Volume`

E,g, **20230101 170401067,1.069700,1.070920,0**

The date part is YYYMMDD and the time part is HHMMSSsss. Bid is the bid price and Ask is the ask price, a double value. Volume is an integer. The input file should not contain any headings.

When producing timeframe files, the output is:

`Datetime, Open, High, Low, Close, Volume`

In this file the Datetime is the number of seconds since epoch.

## Command Line Options

The following command line options are available. Some are mandatory, and some work together depending on the command given. A hort description is available with the help option (-h|--help). When starting the program, you have to give it a command, which determines the mode it will operate in. The different commands are:

1. timeframe - Generate timeframe files from tick input files.
1. config    - Do the configuration using a terminal user interface.
1. run       - This will run the configured system and produce a report.

List of command line options:

| Short | Long version | Description |
|-------|--------------|-------------|
| -h    | --help       | Print the help message. |
| -t    | --timeframe       | Print the help message. |
| -h    | --help       | Print the help message. |
| -h    | --help       | Print the help message. |
| -h    | --help       | Print the help message. |
