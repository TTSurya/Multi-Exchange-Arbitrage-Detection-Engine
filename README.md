
# Multi-Exchange Arbitrage Detection Engine

A graph-based arbitrage detection engine that models cryptocurrency markets as a directed weighted graph and detects profitable trading cycles using the Bellman-Ford algorithm.

## Overview

Financial markets occasionally exhibit temporary pricing inconsistencies across exchanges.

For example:

```text
Buy BTC on Binance
Transfer BTC to Coinbase
Sell BTC on Coinbase
```

If the final capital exceeds the initial capital after accounting for fees and transfer costs, an arbitrage opportunity exists.

This project automatically identifies such opportunities by representing markets as a graph and searching for negative cycles.

---

## Core Idea

Suppose a sequence of trades has exchange rates:

```math
r_1,r_2,\dots,r_n
```

An arbitrage opportunity exists when:

```math
r_1r_2\dots r_n > 1
```

Since graph algorithms operate on additive costs rather than multiplicative gains, every rate is transformed as:

```math
w=-\log(r)
```

```math
-\log(r_1r_2\dots r_n)
=
-\log(r_1)-\log(r_2)-\dots-\log(r_n)
```

Therefore:

```math
r_1r_2\dots r_n > 1
```

is equivalent to:

```math
w_1+w_2+\dots+w_n < 0
```

A profitable arbitrage cycle becomes a negative cycle in the graph.

---

## Graph Representation

Each node represents a state of capital:

```text
Asset@Exchange
```

Examples:

```text
USDT@Binance
BTC@Binance
ETH@Binance

USDT@Coinbase
BTC@Coinbase
ETH@Coinbase
```

Each directed edge represents an action that transforms capital.

### Trading Edges

```text
USDT@Binance -> BTC@Binance
BTC@Binance -> USDT@Binance
```

### Transfer Edges

```text
BTC@Binance -> BTC@Coinbase
BTC@Coinbase -> BTC@Binance
```

---

## Market Frictions

### Bid-Ask Spread

Buying uses the ask price:

```math
rate=\frac{1}{ask(1+fee)}
```

Selling uses the bid price:

```math
rate=bid(1-fee)
```

This naturally incorporates transaction costs and spread losses.

### Transfer Costs

Moving assets between exchanges incurs a transfer loss:

```math
rate = 1 - transfer\_loss
```

For example:

```text
1 BTC -> 0.999 BTC
```

after a 0.1% transfer cost.

---

## Data Sources

### Binance

Live bid and ask prices are fetched using:

```text
/api/v3/ticker/bookTicker
```

### Coinbase

Live bid and ask prices are fetched using:

```text
/products/<asset>-USD/ticker
```

The project currently supports:

```text
BTC
ETH
```

and can be extended easily to additional assets.

---

## Algorithm

1. Fetch market data.

2. Build graph edges.

3. Convert rates into weights using:

   ```math
   w=-\log(rate)
   ```

4. Run Bellman-Ford.

5. Detect negative cycles.

6. Reconstruct the arbitrage cycle.

7. Compute:

   * cycle product
   * log-sum
   * ending capital
   * percentage profit

---

## Example Output

```text
Cycle:

USDT@Binance
-> BTC@Binance
-> BTC@Coinbase
-> USDT@Coinbase
-> USDT@Binance

Cycle Product: 1.0031

Log Sum: -0.00309

Start Capital: 1000

End Capital: 1003.10

Profit %: 0.31
```

---

## Testing

A synthetic exchange is included in `fetch.py` and can be enabled by uncommenting the corresponding block.

The synthetic exchange intentionally introduces profitable pricing discrepancies and serves as a deterministic test case for:

* Bellman-Ford correctness
* cycle reconstruction
* profit calculations
* report generation

When disabled, the engine operates entirely on live market data.

---

## Assumptions

The current model assumes:

* static prices during execution
* infinite liquidity at quoted bid/ask
* no market impact
* no order-book depth constraints
* fixed transfer costs
* negligible network latency

These assumptions simplify the problem while preserving the essential arbitrage-detection logic.

---

## Complexity

Bellman-Ford:

```text
Time Complexity: O(VE)
Space Complexity: O(V)
```

where:

```text
V = Asset@Exchange nodes
E = Trading + Transfer edges
```

---

## Technologies

* C++
* Python
* Bellman-Ford Algorithm
* Graph Modeling
* REST APIs
* CSV-Based Data Pipeline
