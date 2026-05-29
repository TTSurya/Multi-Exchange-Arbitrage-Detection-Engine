import requests,csv

BINANCE_FEE=0.001
COINBASE_FEE=0.002
TEST_FEE=0.0005

TRANSFER_LOSS=0.001

symbols=["BTC","ETH"]

rows=[]

for asset in symbols:

    r=requests.get(f"https://api.binance.com/api/v3/ticker/bookTicker?symbol={asset}USDT").json()

    bid=float(r["bidPrice"])
    ask=float(r["askPrice"])

    rows.append([
        "USDT@Binance",
        f"{asset}@Binance",
        1.0/(ask*(1+BINANCE_FEE))
    ])

    rows.append([
        f"{asset}@Binance",
        "USDT@Binance",
        bid*(1-BINANCE_FEE)
    ])

for asset in symbols:

    r=requests.get(f"https://api.exchange.coinbase.com/products/{asset}-USD/ticker").json()

    bid=float(r["bid"])
    ask=float(r["ask"])

    rows.append([
        "USDT@Coinbase",
        f"{asset}@Coinbase",
        1.0/(ask*(1+COINBASE_FEE))
    ])

    rows.append([
        f"{asset}@Coinbase",
        "USDT@Coinbase",
        bid*(1-COINBASE_FEE)
    ])

    rows.append([
        f"{asset}@Binance",
        f"{asset}@Coinbase",
        1-TRANSFER_LOSS
    ])

    rows.append([
        f"{asset}@Coinbase",
        f"{asset}@Binance",
        1-TRANSFER_LOSS
    ])

#
# Synthetic exchange
#

# test_prices={
#     "BTC":{"bid":115000,"ask":115050},
#     "ETH":{"bid":7000,"ask":7010}
# }

# for asset,p in test_prices.items():

#     rows.append([
#         "USDT@TestExchange",
#         f"{asset}@TestExchange",
#         1.0/(p["ask"]*(1+TEST_FEE))
#     ])

#     rows.append([
#         f"{asset}@TestExchange",
#         "USDT@TestExchange",
#         p["bid"]*(1-TEST_FEE)
#     ])

#     rows.append([
#         f"{asset}@Binance",
#         f"{asset}@TestExchange",
#         1-TRANSFER_LOSS
#     ])

#     rows.append([
#         f"{asset}@TestExchange",
#         f"{asset}@Binance",
#         1-TRANSFER_LOSS
#     ])

with open("rates.csv","w",newline="") as f:
    w=csv.writer(f)
    w.writerow(["from","to","rate"])
    w.writerows(rows)

print("rates.csv generated")