#include "language.h"

const char *languages[][2] = {
    // layout2.c
    {" decrypt for:", "解密"},
    // layout2.c
    {" login to:", "登录到"},
    {" to", "到"},
    // recovery.c
    {"##th", "第##个"},
    // fsm_msg_coin.h
    {"Abort", "取消"},
    // menu.c
    {"About", "关于设备"},
    // layout2.c
    {"Absolute Levy", ""},
    // layout2.c
    {"Acitve", "激活"},
    // fsm.c
    {"Action cancelled by user", ""},  // msg
    // fsm_msg_coin.h fsm_msg_lisk.h
    {"Address:", "地址"},
    // layout2.c
    {"Again", "再次"},
    // fsm_msg_common.h
    {"All data will be lost.", "所有数据将丢失"},
    // u2f.c
    {"Already registered.", "已注册"},
    // u2f.c
    {"Another U2F device", "另外的U2F设备"},
    // u2f.c
    {"Authenticate", "认证"},
    // fsm_msg_common.h
    {"Auto-lock delay too long", ""},  // msg
    // fsm_msg_common.h
    {"Auto-lock delay too short", ""},  // msg
    // layout2.c
    {"BACKUP FAILED!", "备份失败"},
    // layout2.c
    {"BLE Name:", "蓝牙名称:"},
    // layout2.c
    {"BLE enable:", "使用蓝牙:"},
    // layout2.c
    {"BLE version:", "蓝牙版本:"},
    // recovery.c
    {"Back", "后退"},
    // layout2.c
    {"Backup Only", "仅备份"},
    // layout2.c layout2.c
    {"Bluetooth", "蓝牙"},
    // signing.c
    {"Both address and address_n provided.", ""},  // msg
    // signing.c
    {"Branch ID must be set.", ""},  // msg
    // signing.c signing.c
    {"Branch ID not enabled on this coin.", ""},  // msg
    // fsm.c
    {"Button expected", ""},  // msg
    // recovery.c reset.c
    {"By continuing you", " "},
    // fsm_msg_common.h
    {"Can't change se setting after device initialized", ""},  // msg
    // fsm_msg_coin.h
    {"Can't encode address", ""},  // msg
    // ethereum.c ethereum.c ethereum.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    // layout2.c layout2.c layout2.c lisk.c lisk.c
    // lisk.c lisk.c lisk.c lisk.c c c
    // protect.c recovery.c reset.c reset.c signing.c
    // trezor.c fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h
    {"Cancel", "取消"},
    // ethereum.c
    {"Chain Id out of bounds", ""},  // msg
    //
    {"Change PIN", "修改密码"},
    // layout2.c
    {"Change language to", "设置语言为"},
    // layout2.c
    {"CoSi commit index #?", ""},
    // layout2.c
    {"CoSi commit message?", ""},
    // layout2.c
    {"CoSi sign index #?", ""},
    // layout2.c
    {"CoSi sign message?", ""},
    // fsm_msg_coin.h
    {"Computing address", "生成地址中"},
    // ethereum.c ethereum.c ethereum.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    // layout2.c layout2.c lisk.c lisk.c lisk.c lisk.c
    // lisk.c lisk.c c protect.c protect.c
    // protect.c
    // recovery.c recovery.c recovery.c recovery.c reset.c
    // signing.c fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    {"Confirm", "确认"},
    // layout2.c
    {"Confirm OMNI Transaction:", "确认 OMNI 交易"},
    // layout2.c
    {"Confirm OP_RETURN:", "确认 OP_RETURN"},
    // layout2.c lisk.c
    {"Confirm address?", "确认地址"},
    // layout2.c lisk.c
    {"Confirm sending", "确认发送"},
    // lisk.c lisk.c lisk.c lisk.c
    {"Confirm transaction", "确认交易"},
    // layout2.c layout2.c layout2.c
    {"Confirm transfer of", "确认转移"},
    // layout2.c lisk.c reset.c fsm_msg_coin.h
    {"Continue", "继续"},
    // fsm_msg_coin.h
    {"Continue at your", "继续"},
    // fsm_msg_common.h
    {"Continue only if you", "继续"},
    // layout2.c
    {"Continue?", "继续"},
    // layout2.c
    {"Create wallet", "创建新钱包"},
    // fsm.c
    {"Data error", "数据错误"},
    // ethereum.c
    {"Data length exceeds limit", ""},  // msg
    // ethereum.c
    {"Data length provided, but no initial chunk", ""},  // msg
    // signing.c
    {"Decred details provided but Decred coin not specified.", ""},  // msg
    // signing.c
    {"Decred script version does not match previous output", ""},  // msg
    // layout2.c
    {"Decrypt for:", "解密"},
    // layout2.c
    {"Decrypt value of this key?", "为该值解密"},
    // layout2.c
    {"Decrypted message", "解密消息"},
    // layout2.c
    {"Decrypted signed message", "解密签名消息"},
    // layout2.c
    {"Device ID:", "设备ID:"},
    // reset.c reset.c
    {"Device failed initialized", ""},  // msg
    // fsm.c
    {"Device is already initialized. Use Wipe first.", ""},  // msg
    // fsm_msg_common.h fsm_msg_common.h
    {"Device loaded", ""},  // msg
    // config.c fsm.c
    {"Device not initialized", ""},  // msg
    // recovery.c
    {"Device recovered", ""},  // msg
    // reset.c reset.c reset.c fsm_msg_common.h
    {"Device successfully initialized", ""},  // msg
    // fsm_msg_common.h
    {"Device wiped", ""},  // msg
    // layout2.c lisk.c recovery.c reset.c trezor.c
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    {"Do you really want to", "请确认"},
    // fsm_msg_common.h
    {"Do you want to", "请确认"},
    // layout2.c
    {"Do you want to decrypt?", "请确认解密"},
    // fsm_msg_common.h
    {"Do you want to set", "请确认设置"},
    // layout2.c
    {"Do you want to sign in?", "请确认登录"},
    //
    {"Download APP", "下载APP"},
    // ethereum.c
    {"Empty data chunk received", ""},  // msg
    // signing.c signing.c
    {"Encountered invalid prevhash", ""},  // msg
    // layout2.c
    {"Encrypt message?", "加密消息"},
    // layout2.c
    {"Encrypt value of this key?", "为这个值加密"},
    // layout2.c
    {"Encrypt+Sign message?", "加密+签名消息"},
    // layout2.c
    {"Encrypted hex data", "加密十六进制数据"},
    // layout2.c
    {"Encrypted message", "加密信息"},
    // layout2.c layout2.c
    {"English", "中文"},
    // protect.c
    {"Enter new wipe code:", "输入新的擦除密码"},
    // fsm_msg_common.h
    {"Entory data error", ""},  // msg
    // fsm_msg_coin.h
    {"Error computing address", ""},  // msg
    // signing.c
    {"Error computing multisig fingerprint", ""},
    // fsm_msg_crypto.h
    {"Error getting ECDH session key", ""},  // msg
    // fsm_msg_crypto.h
    {"Error signing identity", ""},  // msg
    // fsm_msg_coin.h
    {"Error signing message", ""},  // msg
    // signing.c
    {"Expected input with amount", ""},  // msg
    // signing.c fsm_msg_coin.h
    {"Expiry not enabled on this coin.", ""},  // msg
    // signing.c
    {"Extra data not enabled on this coin.", ""},  // msg
    // signing.c signing.c signing.c signing.c
    // signing.c
    // signing.c
    {"Failed to compile input", ""},  // msg
    // signing.c signing.c signing.c
    {"Failed to compile output", ""},  // msg
    // fsm.c
    {"Failed to derive private key", ""},  // msg
    // signing.c
    {"Failed to serialize extra data", ""},  // msg
    // signing.c signing.c signing.c
    {"Failed to serialize input", ""},  // msg
    // signing.c
    {"Failed to serialize multisig script", ""},  // msg
    // signing.c signing.c
    {"Failed to serialize output", ""},  // msg
    // recovery.c reset.c reset.c fsm_msg_common.h
    // fsm_msg_common.h
    {"Failed to store mnemonic", ""},  // msg
    // menu.c
    {"Fastpay", "快捷支付"},
    // layout2.c
    {"Fee", "手续费"},
    // layout2.c
    {"Fee included:", "已包含手续费"},
    // layout2.c
    {"Finish", "完成"},
    // fsm.c
    {"Firmware error", "固件错误"},
    // layout2.c
    {"Firmware version:", "固件版本:"},
    // fsm_msg_common.h
    {"Flags applied", ""},  // msg
    //
    {"Font:", "字体:"},
    //
    {"For more information", "了解更多"},
    // fsm_msg_common.h
    {"Forbidden field set in dry-run", ""},  // msg
    // layout2.c
    {"GPG sign for:", ""},
    // layout2.c c fsm_msg_common.h
    {"I take the risk", "确认风险"},
    // recovery.c
    {"INVALID!", "不可用"},
    // fsm_msg_common.h
    {"If import seed,", "如果导入种子"},
    // layout2.c c
    {"Import mnemonic", "恢复钱包"},
    // fsm_msg_ethereum.h
    {"Invalid address", ""},  // msg
    // signing.c
    {"Invalid amount specified", ""},  // msg
    // fsm.c
    {"Invalid coin name", ""},  // msg
    // fsm_msg_coin.h
    {"Invalid combination of coin and script_type", ""},  // msg
    // fsm_msg_crypto.h
    {"Invalid global commitment", ""},  // msg
    // fsm_msg_crypto.h
    {"Invalid global pubkey", ""},  // msg
    // fsm_msg_crypto.h fsm_msg_crypto.h
    {"Invalid identity", ""},  // msg
    // lisk.c
    {"Invalid recipient_id", ""},  // msg
    // fsm_msg_common.h
    {"Invalid seed strength", ""},  // msg
    // recovery.c
    {"Invalid seed, are words in correct order?", ""},  // msg
    // fsm.c fsm_msg_coin.h fsm_msg_ethereum.h fsm_msg_lisk.h
    {"Invalid signature", ""},  // msg
    // ethereum.c
    {"Invalid size of initial chunk", ""},  // msg
    // lisk.c lisk.c
    {"Invalid transaction type", ""},  // msg
    // fsm_msg_common.h
    {"Invalid word count", ""},  // msg
    // layout2.c
    {"Label:", "设备名称:"},
    // layout2.c layout2.c
    {"Language", "语言设置"},
    // layout2.c
    {"Levy is", ""},
    // fsm_msg_common.h
    {"Loading private seed", "导入私钥"},
    // trezor.c
    {"Lock Device", "锁定设备"},
    // layout2.c
    {"Login to:", "登录到"},
    // ethereum.c
    {"Malformed address", ""},
    // ethereum.c
    {"Malformed signature", ""},
    // layout2.c
    {"Manual", "用户手册"},
    // layout2.c lisk.c
    {"Message signed by:", ""},
    // messages.c
    {"Message too big", ""},
    // fsm_msg_coin.h fsm_msg_ethereum.h fsm_msg_lisk.h
    {"Message verified", ""},
    // signing.c
    {"Missing address", ""},
    // signing.c signing.c signing.c
    {"Mixing segwit and non-segwit inputs is not allowed", ""},
    //
    {"Mnemonic", "助记词"},
    // fsm_msg_common.h fsm_msg_common.h
    {"Mnemonic with wrong checksum provided", ""},
    // layout2.c
    {"Mosaic", ""},
    // layout2.c
    {"Mosaic Description", ""},
    // signing.c signing.c
    {"Multisig field provided but not expected.", ""},
    // layout2.c
    {"Need Backup", "未备份"},
    // layout2.c
    {"Next", "下一个"},
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    // layout2.c c c c c c
    {"No", "否"},
    // fsm_msg_coin.h fsm_msg_ethereum.h
    {"No address provided", ""},
    // fsm_msg_crypto.h fsm_msg_crypto.h
    {"No data provided", ""},
    // fsm_msg_crypto.h
    {"No key provided", ""},
    // fsm_msg_coin.h fsm_msg_ethereum.h
    {"No message provided", ""},
    // protect.c
    {"No passphrase provided. Use empty string to set an empty passphrase.",
     ""},
    // fsm_msg_common.h
    {"No setting provided", ""},
    // fsm_msg_coin.h
    {"No transaction provided", ""},
    // fsm_msg_crypto.h
    {"No value provided", ""},
    // layout2.c
    {"Not Actived", "未激活"},
    // fsm.c signing.c
    {"Not enough funds", ""},
    // signing.c
    {"Not enough outputs in previous transaction.", ""},
    // ethereum.c
    {"Not in Ethereum signing mode", ""},
    // recovery.c
    {"Not in Recovery mode", ""},
    // reset.c
    {"Not in Reset mode", ""},
    // signing.c
    {"Not in Signing mode", ""},
    // u2f.c
    {"Not registered.", ""},
    // layout2.c u2f.c u2f.c fsm_msg_common.h
    {"OK", "确认"},
    // signing.c
    {"OP RETURN data provided but not OP RETURN script type.", ""},
    // signing.c
    {"OP_RETURN output with address or multisig", ""},
    // signing.c
    {"OP_RETURN output with non-zero amount", ""},
    //
    {"Off", "关"},
    //
    {"On", "开"},
    // signing.c
    {"Output's address_n provided but not expected.", ""},
    // protect.c protect.c protect.c protect.c protect.c
    // protect.c
    {"PIN REF Table", "密码对照表"},
    // fsm.c
    {"PIN cancelled", ""},
    // fsm_msg_common.h
    {"PIN changed", "密码修改成功"},
    // fsm.c
    {"PIN expected", ""},
    // fsm.c
    {"PIN invalid", "密码不正确"},
    // fsm.c
    {"PIN mismatch", "两次输入不相同"},
    // fsm_msg_common.h fsm_msg_common.h
    {"PIN removed", ""},
    //
    {"Page down", "下一页"},
    //
    {"Page up", "上一页"},
    // layout2.c
    {"Path: m", ""},
    // layout2.c
    {"Percentile Levy", ""},
    // layout2.c
    {"Please check the seed", "请检查助记词"},
    // protect.c protect.c
    {"Please confirm PIN", "请确认密码"},
    // protect.c
    {"Please enter currnet PIN", "请输入当前密码"},
    // protect.c
    {"Please enter new PIN", "请输入新密码"},
    // recovery.c
    {"Please enter the", "请输入"},
    // protect.c
    {"Please enter your", "请输入你的"},
    // protect.c
    {"Please enter your PIN:", "请输入密码"},
    // layout2.c
    {"Please input PIN", "请输入密码"},
    // protect.c
    {"Please re-enter new PIN", "请再次输入新密码"},
    // bl_check.c
    {"Please reconnect", ""},
    // recovery.c
    {"Please select the", "请选择"},
    // protect.c
    {"Please wait", "请等待"},
    // fsm.c
    {"Process error", ""},
    // storage.c
    {"Processing", "处理中"},
    // signing.c
    {"Pubkey not found in multisig script", ""},
    // lisk.c
    {"Public Key:", "公钥"},
    // layout2.c
    {"QR Code", "二维码"},
    // layout2.c
    {"Quota:", "单次限额"},
    // layout2.c
    {"Raw levy value is", ""},
    // protect.c
    {"Re-enter new wipe code:", ""},
    // ethereum.c layout2.c
    {"Really send", ""},
    // u2f.c
    {"Register", "注册"},
    // layout2.c
    {"Remaining times:", "剩余次数"},
    //
    {"Reset", "重置设备"},
    //
    {"Retry", "重试"},
    // layout2.c
    {"SE version:", "SE版本:"},
    // layout2.c
    {"SEEDLESS", ""},
    // ethereum.c
    {"Safety check failed", ""},
    //
    {"Security", "安全"},
    // reset.c
    {"Seed already backed up", ""},
    // reset.c
    {"Seed successfully backed up", ""},
    // signing.c
    {"Segwit input without amount", ""},
    // signing.c
    {"Segwit not enabled on this coin", ""},
    // layout2.c
    {"Select mnemonic number", "选择助记词个数"},
    // ethereum.c
    {"Send", ""},
    // layout2.c
    {"Send anyway?", ""},
    // layout2.c
    {"Serial:", "序列号:"},
    // fsm_msg_common.h
    {"Session cleared", ""},
    // layout2.c
    {"Set auto power off time", "设置自动关机时间"},
    // menu.c
    {"Settings", "设置"},
    // fsm_msg_common.h
    {"Settings applied", ""},
    // layout2.c layout2.c
    {"Shutdown", "关机时间"},
    // layout2.c
    {"Sign binary message?", ""},
    // layout2.c
    {"Sign message?", "签名消息"},
    // ethereum.c ethereum.c ethereum.c ethereum.c lisk.c
    {"Signing", "签名中"},
    // signing.c
    {"Signing error", ""},
    // ethereum.c ethereum.c signing.c
    {"Signing failed", ""},
    // lisk.c signing.c signing.c signing.c signing.c
    // signing.c signing.c signing.c signing.c
    {"Signing transaction", "签名交易中"},
    // layout2.c
    {"Simple send of ", ""},
    // fsm_msg_coin.h fsm_msg_crypto.h
    {"Singing", ""},
    // layout2.c
    {"Skip button confirm:", "跳过确认"},
    // layout2.c
    {"Skip pin check:", "免密支付"},
    // storage.c
    {"Starting up", "启动中"},
    // config.c
    {"Storage failure", ""},
    // protect.c
    {"The new PIN must be different from your wipe code.", ""},
    // recovery.c
    {"The seed is", "种子"},
    // recovery.c recovery.c
    {"The seed is valid", ""},
    // recovery.c
    {"The seed is valid and matches the one in the device", ""},
    // recovery.c
    {"The seed is valid but does not match the one in the device", ""},
    // protect.c
    {"The wipe code must be different from your PIN.", ""},
    // u2f.c
    {"This U2F device is", ""},
    // protect.c fsm_msg_common.h
    {"This firmware is incapable of passphrase entry on the device.", ""},
    // signing.c fsm_msg_coin.h
    {"Timestamp must be set.", ""},
    // signing.c fsm_msg_coin.h
    {"Timestamp not enabled on this coin.", ""},
    // ethereum.c
    {"Too much data", ""},
    // ethereum.c
    {"Transaction data:", ""},
    // signing.c signing.c signing.c
    {"Transaction has changed during signing", ""},
    // fsm_msg_coin.h
    {"Transaction must have at least one input", ""},
    // fsm_msg_coin.h
    {"Transaction must have at least one output", ""},
    // ethereum.c
    {"Txtype out of bounds", ""},
    // fsm_msg_common.h
    {"U2F counter set", ""},
    // layout2.c
    {"U2F security key?", ""},
    // layout2.c
    {"Unencrypted hex data", ""},
    // layout2.c
    {"Unencrypted message", ""},
    // fsm.c
    {"Unexpected message", ""},
    // layout2.c
    {"Unknown Mosaic", ""},
    // bl_check.c
    {"Unknown bootloader", ""},
    // layout2.c
    {"Unknown long path", ""},
    // messages.c messages.c
    {"Unknown message", ""},
    // layout2.c
    {"Unknown transaction", ""},
    // bl_check.c bl_check.c
    {"Unplug your Trezor", ""},
    // config.c
    {"Unsupported curve", ""},
    // signing.c
    {"Unsupported transaction version.", ""},
    // signing.c
    {"Unsupported version for overwintered transaction", ""},
    // bl_check.c
    {"Update finished", ""},
    // config.c
    {"Updating", "更新"},
    // bl_check.c
    {"Updating bootloader", ""},
    // layout2.c
    {"Use SE:", "使用SE:"},
    // fsm_msg_crypto.h
    {"Value length must be a multiple of 16", ""},
    // signing.c signing.c signing.c signing.c signing.c
    // signing.c fsm_msg_coin.h
    {"Value overflow", ""},
    // layout2.c
    {"Verified binary message", ""},
    // layout2.c
    {"Verified message", ""},
    // fsm_msg_coin.h
    {"Verifying", ""},
    // storage.c
    {"Verifying PIN", "校验PIN"},
    // signing.c
    {"Version group ID must be set when version >= 3.", ""},
    // signing.c
    {"Version group ID must be set.", ""},
    // signing.c signing.c
    {"Version group ID not enabled on this coin.", ""},
    // config.c
    {"Waking up", "唤醒"},
    // layout2.c
    {"Warning!", "警告"},
    // layout2.c
    {"Web sign in to:", ""},
    // fsm_msg_common.h
    {"Wipe code changed", ""},
    // fsm.c
    {"Wipe code mismatch", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"Wipe code removed", ""},
    // fsm_msg_common.h
    {"Wipe code set", ""},
    // recovery.c
    {"Word not found in a wordlist", ""},
    // layout2.c
    {"Write down the seed", "请抄写助记词"},
    // fsm_msg_coin.h
    {"Wrong address path", ""},
    // signing.c signing.c
    {"Wrong input script type", ""},
    // recovery.c
    {"Wrong word retyped", ""},
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    {"Yes", "是"},
    // layout2.c
    {"active device", "激活设备"},
    // fsm_msg_common.h
    {"addres is existed", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"addres is null", ""},
    // fsm_msg_common.h
    {"addres space is full", ""},
    // layout2.c
    {"after ", ""},
    // recovery.c reset.c
    {"agree to trezor.io/tos", ""},
    // u2f.c
    {"already registered", "已注册"},
    // recovery.c
    {"and MATCHES", ""},
    // layout2.c
    {"and levy of", ""},
    // layout2.c c c
    {"and network fee of", ""},
    // fsm_msg_common.h
    {"answer to ping?", ""},
    // layout2.c
    {"auto-lock your device", ""},
    // fsm_msg_common.h
    {"backup only", "仅备份"},
    // bl_check.c
    {"broken.", ""},
    // recovery.c
    {"but does NOT MATCH", ""},
    // fsm_msg_common.h
    {"change bluetooth", ""},
    // fsm_msg_common.h
    {"change current PIN?", "修改密码"},
    // fsm_msg_common.h
    {"change fastpay settings", "修改快捷支付"},
    // fsm_msg_common.h
    {"change language to", "修改语言为"},
    // fsm_msg_common.h
    {"change name to", "修改名称为"},
    // fsm_msg_common.h
    {"change the current", ""},
    // fsm_msg_common.h
    {"change the home screen", "修改屏保"},
    // layout2.c
    {"change-outputs.", ""},
    // signing.c
    {"changed", "修改"},
    // bl_check.c bl_check.c
    {"contact our support.", ""},
    // reset.c
    {"create a new wallet?", "创建新的钱包?"},
    // bl_check.c config.c
    {"detected.", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"device initialied success", "设备初始化成功"},
    // fsm_msg_common.h
    {"device is used for", "设备用于"},
    // fsm_msg_common.h
    {"disable passphrase", "禁用密语"},
    // fsm_msg_common.h
    {"disable wipe code", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"doing!", "操作"},
    // fsm_msg_common.h
    {"enable passphrase", "使用密语"},
    // lisk.c
    {"fee:", "手续费"},
    // ethereum.c
    {"for gas?", ""},
    // fsm_msg_coin.h
    {"for selected coin.", ""},
    // layout2.c
    {"from your wallet?", "从钱包中"},
    // layout2.c
    {"hour", "小时"},
    // recovery.c
    {"imported succeed", "导入成功"},
    // layout2.c
    {"in", ""},
    // layout2.c
    {"in raw units of", ""},
    // layout2.c
    {"in the same mosaic", ""},
    // u2f.c u2f.c
    {"in this application.", ""},
    // fsm_msg_common.h
    {"increase and retrieve", ""},
    // fsm_msg_common.h
    {"is not recommended.", ""},
    // layout2.c
    {"is unexpectedly high.", "过高"},
    // protect.c
    {"keyboard.", "键盘"},
    // fsm_msg_common.h fsm_msg_common.h
    {"know what you are", "清楚你的"},
    // trezor.c
    {"lock your Trezor?", "锁定设备"},
    // ethereum.c ethereum.c
    {"message", "消息"},
    // layout2.c
    {"minute", "分"},
    // recovery.c
    {"of your mnemonic", "助记词"},
    // recovery.c
    {"on your computer", "在电脑上"},
    // fsm_msg_coin.h
    {"own risk!", ""},
    // protect.c
    {"passphrase using", "密语"},
    // ethereum.c
    {"paying up to", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"protection?", "保护"},
    // layout2.c
    {"raw units of", ""},
    // fsm_msg_common.h
    {"reboot start", ""},
    // recovery.c
    {"recover the device?", "恢复钱包"},
    // lisk.c
    {"register a delegate?", ""},
    // fsm_msg_common.h
    {"remove current PIN?", ""},
    // fsm_msg_common.h
    {"restore data err", ""},
    // layout2.c
    {"s", "秒"},
    // layout2.c protect.c
    {"second", "秒"},
    // fsm_msg_common.h
    {"seed import failed", "种子导入失败"},
    // fsm_msg_common.h
    {"seed import success", "种子导入成功"},
    // signing.c
    {"segwit input amount", ""},
    // fsm_msg_common.h
    {"send entropy?", "发送熵"},
    // fsm_msg_common.h
    {"set a new wipe code?", ""},
    // fsm_msg_common.h
    {"set new PIN?", "设置密码"},
    // fsm_msg_common.h
    {"status always?", ""},
    // bl_check.c
    {"successfully.", "成功"},
    // fsm_msg_common.h fsm_msg_common.h
    {"the U2F counter?", ""},
    // protect.c
    {"the computer's", "使用电脑的"},
    // recovery.c recovery.c
    {"the one in the device.", ""},
    // layout2.c
    {"the same mosaic", ""},
    // protect.c
    {"to continue ...", " "},
    // ethereum.c
    {"to new contract?", "新合约"},
    // lisk.c
    {"to:", ""},
    // ethereum.c
    {"token", "代币"},
    // layout2.c layout2.c
    {"user: ", "用户:"},
    // u2f.c
    {"was used to register", "注册"},
    // fsm_msg_common.h
    {"wipe code?", "擦除密码"},
    // fsm_msg_common.h
    {"wipe the device?", "擦除设备"},

    {"word", "单词"},
};

int LANGUAGE_ITEMS = sizeof(languages) / sizeof(languages[0]);
