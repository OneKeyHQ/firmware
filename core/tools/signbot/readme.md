# ed25519 cosign slack bot #

1. 初始化venv
```
cd firmware/core/tools/signbot
python3 -m venv .venv
source .venv/bin/activate
```
2. 安装trezorlib 
```
cd firmware
pip install ../../../python
```
3. 安装其他
```
pip install -r requirements.txt
```
4. 修改run.sh，填入SLACK_APP_TOKEN和SLACK_BOT_TOKEN
5. 运行bot
```
bash run.sh
```
