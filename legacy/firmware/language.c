#include "language.h"

const char *languages[][2] = {
    //
    {" Off", "已关闭"},
    //
    {" On", "已开启"},
    // layout2.c
    {" decrypt for:", "解密"},
    // fsm.c
    {" is a non-standard path. Are you sure to use this path?",
     " 是非标准路径, 确定要继续使用该路径吗?"},
    // layout2.c
    {" login to:", "登录到"},
    //
    {" to", "到"},
    // layout2.c
    {"(Recommended.)", "(推荐开启.)"},
    // recovery.c
    {"##th", "第##个"},
    {"2 wrong attemps,\nyou have 8 more\nchance",
     "已累计输错2次,还有8\n次输入机会"},
    {"3 wrong attemps,\nyou have 7 more\nchance",
     "已累计输错3次,还有7\n次输入机会"},
    {"4 wrong attemps,\nyou have 6 more\nchance",
     "已累计输错4次,还有6\n次输入机会"},
    {"5 wrong attemps,\nyou have 5 more\nchance",
     "已累计输错5次,还有5\n次输入机会"},
    {"6 wrong attemps,\nyou have 4 more\nchance",
     "已累计输错6次,还有4\n次输入机会"},
    {"7 wrong attemps,\nyou have 3 more\nchance",
     "已累计输错7次,还有3\n次输入机会"},
    {"8 wrong attemps,\nyou have 2 more\nchance",
     "已累计输错8次,还有2\n次输入机会"},
    {"9 wrong attemps,\nyou have 1 more\nchance",
     "已累计输错9次,还有1\n次输入机会"},
    {"APPROVE", "授权"},
    // fsm_msg_coin.h
    {"Abort", "取消"},
    // menu.c
    {"About", "关于本机"},
    // layout2.c
    {"Absolute Levy", ""},
    //
    {"Access hidden wallet?", "使用隐藏钱包?"},
    // layout2.c
    {"Acitve", "激活"},
    // fsm.c
    {"Action cancelled by user", ""},  // msg
    // fsm_msg_coin.h fsm_msg_lisk.h
    {"Address:", "地址"},
    //
    {"Advance CFX Sign", "CFX高级签名"},
    //
    {"Advance ETH Sign", "ETH高级签名"},
    //
    {"Advance SOL Sign", "SOL高级签名"},
//
//
#if ONEKEY_MINI
    {"After enabling\"Blind \nSigning\",your device \nwill support signing \n"
     "for messages and \ntransactions, but it \ncan't decode the metadata.",
     "开启盲签后,设备将支持\n对消息或交易进行签名,\n但不会解析元数据"},
#else
    {"After enabling \n\"Blind Signing\",your device  will support signing for "
     "messages and transactions, but it can't decode the metadata.",
     "\n开启盲签后,设备将支持对消息或交易进行签名,但不会解析元数据"},
#endif

    // layout2.c
    {"Again", "再次"},
    // menu_list.c
    {"All data on the device\nwill be erased if the\ndevice is reset, "
     "and\nthis operation cannot\nbe reversed.",
     "重置设备将抹除本机全部数\n据,本操作不可撤销"},

    // fsm_msg_common.h
    {"All data will be lost.", "所有数据将丢失"},
    // u2f.c
    {"Already registered.", "已注册."},
    {"Amount", "金额"},
    // u2f.c
    {"Another U2F device", "另一个 U2F 设备"},
    //
    {"Are you sure to reset?", "确定要重置吗?"},
    //
    {"Are you sure to update?", "确定升级固件吗?"},
    {"Are you sure you want\nto reset your device?", "确定要重置吗?"},
    // ada.c
    {"Asset Fingerprint:", "资产指纹:"},
    // u2f.c
    {"Authenticate", "认证"},
    {"Authorized by", "授权方"},
    // fsm_msg_common.h
    {"Auto-lock delay too long", ""},  // msg
    // fsm_msg_common.h
    {"Auto-lock delay too short", ""},  // msg
    //
    {"AutoLock", "锁定时间"},
    {"BACK", "返回"},
    // layout2.c
    {"BACKUP FAILED!", "备份失败"},
    // tron.c
    {"BANDWIDTH", "带宽"},
    // layout2.c
    {"BLE Name:", "蓝牙名称:"},
    // layout2.c
    {"BLE enable:", "使用蓝牙:"},
    // layout2.c
    {"BLE version:", "蓝牙版本:"},
    // layout2.c
    {"BUILD ID:", "构建号:"},
    // recovery.c
    {"Back", "返回"},
    // layout2.c
    {"Backup Only", "仅备份"},
    // tron.c
    {"Balance:", "金额:"},
    // menu_list.c
    {"Before you reset the\ndevice, please ensure\nyou've backed up "
     "your\nrecovery phrase",
     "在重置设备前,请确保您仍\n掌握该助记词的物理备份"},
    //
    {"Before your start", "继续操作前"},
    //
    {"Blind Signing", "盲签"},
    // layout2.c layout2.c
    {"Bluetooth", "蓝牙"},
    // signing.c
    {"Both address and address_n provided.", ""},  // msg
    // signing.c
    {"Branch ID must be set.", ""},  // msg
    // signing.c signing.c
    {"Branch ID not enabled on this coin.", ""},  // msg
    {"Brightness", "亮度"},
    {"Burn tokens", "销毁代币"},
    // fsm.c
    {"Button expected", ""},  // msg
    // recovery.c reset.c
    {"By continuing you", " "},
    // starcoin.c
    {"CANCEL", "取消"},
    {"CONFIRM", "确认"},
    {"CONFIRM SIGNING:", "确认签名:"},
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
    {"Cancel or Back", "取消或返回"},
    // cosmos.c
    {"Chain ID", "链 ID"},
    // ethereum.c
    {"Chain Id out of bounds", ""},  // msg
    // cosmos.c
    {"Chain Name", "链名称"},
    //
    {"Change PIN", "修改PIN码"},
    // layout2.c
    {"Change language to", "设置语言为"},
    //
    {"Check Mnemonic", "查看助记词"},
    //
    {"Check PIN Code", "请先校验设备原PIN码"},
    {"Check Passed", "核对通过"},
    {"Check Recovery Phrase", "助记词核对"},
    //
    {"Check failed", "核对失败"},
    //
    {"Check failed, Quit\nand try again", "核对失败,请退出重试"},
    //
    {"Check passed", "核对通过"},
    //
    {"Check the 12 words\nyou entered are the\ncorrect recovery\nphrase",
     "重新检查已输入的12位单\n词,确保它是正确的助记词"},
    {"Check the 18 words\nyou entered are the\ncorrect recovery\nphrase",
     "重新检查已输入的18位单\n词,确保它是正确的助记词"},
    {"Check the 24 words\nyou entered are the\ncorrect recovery\nphrase",
     "重新检查已输入的24位单\n词,确保它是正确的助记词"},
    //
    {"Check the entered", "请检查输入的"},
    {"Check the recovery\nphrase once more and\ncompare it to the\nbackup copy "
     "in your\nhand. Make sure the\nspelling and order\nare identical.",
     "再次查看助记词,并与手\n中的备份进行核对,确保\n二者完全一致."},
    //
    {"Check the seed", "检查助记词"},
    // layout2.c
    {"Check the written", "请检查写下的"},
    //
    {"Check this device with\nOneKey secure server?",
     "确定访问OneKey服务器对\n此设备进行防伪校验吗?"},
    {"Choose the correct\nwords based on the\nrecovery phrase\nyou've put "
     "down.",
     "接下来,请根据已写下的助\n记词选择正确的单词"},
    {"Click to Unlock", "按任意键解锁"},
    // algorand.c
    {"Close to", "关闭账户并转移资产到"},
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
    // ethereum.c
    {"Confirm fee", "确认交易费"},
    // layout2.c lisk.c
    {"Confirm sending", "确认发送"},
    {"Confirm sending:", "确认发送:"},
    // starcoin.c aptos.c
    {"Confirm signing", "确认签名"},
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
    {"Create", "创建新钱包"},
    {"Create New Wallet", "创建新钱包"},
    {"Create account", "创建账户"},
    {"Create multisig", "创建多重签名"},
    {"DONE", "完成"},
    {"Dashboard", "控制中心"},
    // lnurl.c
    {"Data", "数据"},
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
    {"Decrypt Message", "解密消息"},
    {"Decrypt Nostr Message", "解密 Nostr 消息"},
    {"Decrypt for:", "解密"},
    // layout2.c
    {"Decrypt value of this key?", "为该值解密"},
    // layout2.c
    {"Decrypted message", "解密消息"},
    // layout2.c
    {"Decrypted signed message", "解密签名消息"},
    // cosmos.c
    {"Delegate", "委托"},
    {"Delegator", "代理方"},
    {"Deposit", "存款"},
    {"Deposit Amount", "存款金额"},
    {"Deposit Denomination", "存款面值"},
    {"Depositer", "存款方"},
    {"Description", "描述"},
    {"Destination Address", "目标地址"},
    {"Destination Coins", "来源金额"},
    // layout2.c
    {"Device ID:", "设备ID:"},
    // menu_list.c
    {"Device Info", "设备信息"},
    // reset.c reset.c
    {"Device failed initialized", ""},  // msg
    //
    {"Device has been reset", "设备已重置"},
    // fsm.c
    {"Device is already initialized. Use Wipe first.", ""},  // msg
    // fsm_msg_common.h fsm_msg_common.h
    {"Device loaded", ""},  // msg
    // config.c fsm.c
    {"Device not initialized", ""},  // msg
    // recovery.c
    {"Device recovered", ""},  // msg
    //
    {"Device reset in\nprogress", "设备重置中"},
    {"Device reset in progress", "设备重置中"},
    // reset.c reset.c reset.c fsm_msg_common.h
    {"Device successfully initialized", ""},  // msg
    // fsm_msg_common.h
    {"Device wiped", ""},  // msg
    // menu_list.c
    {"Do not change this\nsetting if you no\nsure.",
     "如果您不确定此操作的后果, 请勿更改此设置."},  // msg
    // layout2.c lisk.c recovery.c reset.c trezor.c
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    // fsm_msg_common.h fsm_msg_common.h fsm_msg_common.h
    {"Do you really want to", "请确认"},
    //
    {"Do you really want to \ndisable passphrase protection?",
     "要禁用passphrase加密吗?"},
    //
    {"Do you really want to \nenable passphrase protection?",
     "要启用passphrase加密吗?"},
    // fsm_msg_common.h
    {"Do you want to", "请确认"},
    // layout2.c
    {"Do you want to decrypt\nNostr message?", "确认要解密本次 Nostr 消息吗?"},
    {"Do you want to decrypt?", "请确认解密"},
    {"Do you want to encrypt\nNostr message?", "确认要加密本次 Nostr 消息吗?"},
    // fsm_msg_common.h
    {"Do you want to set", "请确认设置"},
    // layout2.c
    {"Do you want to sign in?", "请确认登录"},
    {"Do you want to sign this\n", "确认要签署此 "},
    // lnurl
    {"Domain", "域名"},
    //
    {"Done", "完成"},
    {"Download OneKey", "下载OneKey"},
    // menu_list.c
    {"Download OneKey Apps", "官网下载OneKey客户端"},
    // tron.c
    {"ENERGY", "能量"},
    //
    {"ETH advance signing turn", "ETH高级签名"},
    // ethereum.c
    {"Empty data chunk received", ""},  // msg
    // signing.c signing.c
    {"Encountered invalid prevhash", ""},  // msg
    // layout2.c
    {"Encrypt Message", "加密消息"},
    {"Encrypt Nostr Message", "加密 Nostr 消息"},
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
    {"English", "简体中文"},
    //
    {"Enter", "填入"},
    //
    {"Enter PIN again", "再次输入PIN码"},
    //
    {"Enter PIN code", "请输入PIN码"},
    //
    {"Enter PIN to unlock", "输入PIN码解锁"},
    //
    {"Enter Passphrase", "输入Passphrase"},
    // protect.c
    {"Enter new wipe code:", "输入新的擦除PIN码"},
    //
    {"Enter seed phrase ", "输入助记词"},
    //
    {"Enter seed phrases to", "输入助记词"},
    {"Enter word", "输入助记词"},
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
    //
    {"Exit", "退出"},
    // signing.c
    {"Expected input with amount", ""},  // msg
    // signing.c fsm_msg_coin.h
    {"Expiry not enabled on this coin.", ""},  // msg
    // signing.c
    {"Extra data not enabled on this coin.", ""},  // msg
    {"FIRMWARE:", "固件版本:"},
    {"FLASH:", "闪存:"},
    {"FONT:", "字体:"},
    // starcoin.c
    {"FORMAT:", "格式:"},
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
    {"Fee payer", "手续费支付方"},
    // layout2.c
    {"Finish", "完成"},
    // fsm.c
    {"Firmware error", "固件错误"},
    // layout2.c
    {"Firmware version:", "固件版本:"},
    // fsm_msg_common.h
    {"Flags applied", ""},  // msg
    // recovery.c
    {"Follow the guide and\nenter recovery phrase\nto restore wallet.",
     "跟随引导,依次输入助记词\n恢复钱包"},
    //
    // reset.c
    {"Follow the guide to\ncreate a new wallet", "请跟随引导创建新钱包"},
    {"Follow the prompts", "按照屏幕提示"},
    //
    {"Font:", "字体支持:"},
    //
    {"For more information", "了解更多"},
    // fsm_msg_common.h
    {"Forbidden field set in dry-run", ""},  // msg
    {"From", "发送方"},
    // layout2.c
    {"GPG sign for:", ""},
    {"Gas Fee Cap", "燃料单价上限"},
    {"Gas Limit", "燃料用量上限"},
    {"Gas Premium", "优先费用"},
    // cosmos.c
    {"Granter", "授予人"},
    // layout2.c
    {"Guide", "使用教程"},
    {"High", "高"},
    // layout2.c c fsm_msg_common.h
    {"I take the risk", "确认风险"},
    // recovery.c
    {"INVALID!", "不可用"},
    // fsm_msg_common.h
    {"If import seed,", "如果导入种子"},
    //
    {"Inconsistent PIN code", "PIN码前后不一致"},
    {"Incorrect PIN code", "PIN码错误"},
    // recovery.c
    {"Incorrect recovery\nphrase, try again.", "助记词错误,请重试"},
    {"Incorrect recovery phrase. Try again.", "助记词不正确,请重试"},
    {"Incorrect word. Try\nagain", "助记词错误,请重试"},
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
    //
    {"Invalid seed phrases", "助记词无效"},
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
    // menu_list.c
    {"It will take effect\nafter device restart.",
     "将在设备重启后生效."},  // msg
    // layout2.c
    {"Label:", "设备名称:"},
    // layout2.c layout2.c
    {"Language", "语言"},
    // layout2.c
    {"Levy is", ""},
    // fsm_msg_common.h
    {"Loading private seed", "导入私钥"},
    // signing.c
    {"Loading transaction", "加载交易"},
    // trezor.c
    {"Lock Device", "锁定设备"},
    // tron.c
    {"Lock:", "锁定:"},
    //
    {"Locktime for this", "该交易时间锁定"},
    // layout2.c
    {"Login to:", "登录到"},
    {"Low", "低"},
    {"MODEL NAME:", "设备名称:"},
    //
    {"Make sure you still have", "请确保您仍掌握"},
    // ethereum.c
    {"Malformed address", ""},
    // ethereum.c
    {"Malformed signature", ""},
    {"Maximum fee", "交易费上限"},
    {"Maximum fee per gas", "交易费上限单价"},
    {"Medium", "中"},
    // cosmos.c
    {"Memo", "备注"},
    // ethereum.c
    {"Message Hash", "消息哈希值"},
    // layout2.c lisk.c
    {"Message signed by:", "消息签名者:"},
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
    //
    {"Mnemonic verified pass", "助记词校验通过"},
    // fsm_msg_common.h fsm_msg_common.h
    {"Mnemonic with wrong checksum provided", ""},
    // layout2.c
    {"Mosaic", ""},
    // layout2.c
    {"Mosaic Description", ""},
    // cosmos.c
    {"Multi Send", "多签"},
    // signing.c signing.c
    {"Multisig field provided but not expected.", ""},
    {"NEXT", "继续"},
    // layout2.c
    {"Need Backup", "未备份"},
    //
    {"Never", "永不"},
    // layout2.c
    {"Next", "继续"},
    {"Next Page", "下一页"},
    //
    {"Next screen will show", "下一屏幕将显示"},
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
    {"Not registered.", "未注册."},
    // algorand.c
    {"Note", "备注"},
    // layout2.c u2f.c u2f.c fsm_msg_common.h
    {"OK", "确认"},
    {"OK or Next", "确认或继续"},
    // signing.c
    {"OP RETURN data provided but not OP RETURN script type.", ""},
    // signing.c
    {"OP_RETURN output with address or multisig", ""},
    // signing.c
    {"OP_RETURN output with non-zero amount", ""},
    //
    {"Off", "关闭"},
    //
    {"Okay", "确认"},
    //
    {"On", "开启"},
    // cosmos.c
    {"Option", "选项"},
    // signing.c
    {"Output's address_n provided but not expected.", ""},
    // protect.c protect.c protect.c protect.c protect.c
    // fsm.c
    {"PIN cancelled", ""},
    {"PIN changed", "PIN码修改成功"},
    // fsm_msg_common.h
    {"PIN code change", "PIN码修改"},
    //
    {"PIN code set", "PIN设置"},
    // protect.c
    {"PIN does not match,\ntry again", "PIN码前后不一致,请\n重新输入."},
    // fsm.c
    {"PIN expected", ""},
    // fsm.c
    {"PIN invalid", "PIN码错误"},
    {"PIN is used to unlock your device. Please keep it safe.",
     "PIN码用于解锁设备,请妥善保管."},
    // fsm.c
    {"PIN mismatch", "两次输入不相同"},
    // fsm_msg_common.h fsm_msg_common.h
    {"PIN removed", ""},
    {"PIN set", "PIN设置成功"},
    {"PREV", "后退"},
    //
    {"Page down", "下一页"},
    //
    {"Page up", "上一页"},
    // layout2.c
    {"Path: m", ""},
    // layout2.c
    {"Percentile Levy", ""},
    // protect.c protect.c
    {"Please confirm PIN", "请确认PIN码"},
    {"Please copy the\nfollowing 12 words in\norder", "请依次抄写12位助记词"},
    {"Please copy the\nfollowing 18 words in\norder", "请依次抄写18位助记词"},
    {"Please copy the\nfollowing 24 words in\norder", "请依次抄写24位助记词"},
    {"Please enter", "请输入"},
    {"Please enter 12 words\nin order to verify\nyour recovery phrase",
     "请依次输入12位助记词进\n行核对"},
    {"Please enter 18 words\nin order to verify\nyour recovery phrase",
     "请依次输入18位助记词进\n行核对"},
    {"Please enter 24 words\nin order to verify\nyour recovery phrase",
     "请依次输入24位助记词进\n行核对"},
    // protect.c
    {"Please enter current PIN", "请输入当前PIN码"},
    // protect.c
    {"Please enter new PIN", "请输入新PIN码"},
    // recovery.c
    {"Please enter the", "请输入"},
    // protect.c
    {"Please enter your", "请输入你的"},
    // recovery.c
    {"Please enter your 12-\nwords recovery phrase\nin order.",
     "请依次输入12位助记词"},
    {"Please enter your 18-\nwords recovery phrase\nin order.",
     "请依次输入18位助记词"},
    {"Please enter your 24-\nwords recovery phrase\nin order.",
     "请依次输入24位助记词"},
    // protect.c
    {"Please enter your PIN:", "请输入PIN码"},
    // layout2.c
    {"Please input PIN", "请输入PIN码"},
    // protect.c
    {"Please re-enter new PIN", "请再次输入新PIN码"},
    //
    {"Please reboot", "按确认将重启"},
    // bl_check.c
    {"Please reconnect", ""},
    // recovery.c
    {"Please select the", "请选择"},
    //
    {"Please set the PIN", "请设置PIN码"},
    // protect.c
    {"Please set your PIN", "请设置PIN码"},
    {"Please set your PIN.\nPIN is used to unlock\nyour device. Please\nkeep "
     "it safe.",
     "请设置PIN码\nPIN码用于解锁设备,请妥\n善保存."},
    //
    {"Please try again", "请重新输入"},
    // protect.c
    {"Please wait", "请等待"},
    //
    {"Press", "按"},
    //
    {"Press any key to continue", "按任意键继续"},
    //
    {"Prev", "后退"},
    {"Previous Page", "上一页"},
    // ethereum.c
    {"Priority fee per gas", "每份燃料消耗的优先费用"},
    // fsm.c
    {"Process error", ""},
    // storage.c
    {"Processing", "处理中"},
    // cosmos.c
    {"Proposal", "提案"},
    {"Proposal ID", "提案 ID"},
    {"Propose", "投票"},
    {"Proposer", "提案申请人"},
    // signing.c
    {"Pubkey not found in multisig script", ""},
    // lisk.c
    {"Public Key:", "公钥"},
    // layout2.c
    {"QR Code", "二维码"},
    {"QUIT", "退出"},
    // layout2.c
    //
    {"Quit", "退出"},
    {"Quota:", "单次限额"},
    {"REJECT", "拒绝"},
    //
    {"RESTART", "重启"},
    {"RETRY", "重试"},
    // layout2.c
    {"Raw levy value is", ""},
    // protect.c
    {"Re-enter new wipe code:", ""},
    // ethereum.c layout2.c
    {"Really send", "确认发送"},
    {"Receiver", "接收方"},
    {"Receiver:", "接收方:"},
    {"Recipient", "接收方"},
    // reset.c
    {"Recovery Phrase", "助记词"},
    {"Recovery Phrase\nimported.", "助记词导入成功"},
    {"Recovery Phrase\nverified", "助记词校验通过"},
    {"Recovery Phrase\nverified pass", "助记词校验通过"},
    {"Recovery Phrase verify", "助记词核对"},
    // cosmos.c
    {"Redelegate", "重新委托"},
    // u2f.c
    {"Register", "注册"},
    // algorand.c
    {"Rekey to", "重新授权给"},
    // layout2.c
    {"Remaining times:", "剩余次数"},
    //
    {"Reset", "重置设备"},
    //
    {"Reset ", "重置"},
    {"Reset Device?", "确定要重置吗?"},
    {"Reset successfully,\nrestart the device", "重置成功,请重启"},
    {"Reset successfully.\nPlease restart the\ndevice.", "重置成功,请重启."},
    // tron.c
    {"Resource:", "资源:"},
    //
    {"Restore", "恢复钱包"},
    {"Restore Wallet", "恢复钱包"},
    //
    {"Retry", "重试"},
    // layout2.c
    {"SE version:", "SE版本:"},
    // layout2.c
    {"SEEDLESS", ""},
    // starcoin.c
    {"SENDER:", "发送方:"},
    // layout2.c
    {"SERIAL:", "序列号:"},
    //
    {"SOL advance signing turn", "SOL高级签名"},
    // ethereum.c
    {"Safety check failed", ""},
    //
    {"Scan the QR code below", "扫描下方二维码"},
    //
    {"Security", "安全"},
    //
    {"Seed Phrase", "助记词"},
    // reset.c
    {"Seed already backed up", ""},
    // reset.c
    {"Seed successfully backed up", ""},
    // signing.c
    {"Segwit input without amount", ""},
    // signing.c
    {"Segwit not enabled on this coin", ""},
    {"Select correct word below", "按提示选择正确的单词"},
    {"Select the number of\nrecovery phrase word", "选择助记词位数"},
    {"Select the sequence\nnumber of word to be\nchecked",
     "选择要核对的助记词序号"},
    {"Select word", "选择助记词"},
    //
    {"Select your", "选择你的"},
    // ethereum.c
    {"Send", "发送"},
    {"Send SOL", "发送 SOL"},
    // layout2.c
    {"Send anyway?", ""},
    {"Send to", "发送给"},
    {"Send to:", "发送给:"},
    {"Sender", "发送方"},
    // layout2.c
    {"Serial:", "序列号:"},
    // fsm_msg_common.h
    {"Session cleared", ""},
    {"Set PIN", "设置PIN码"},
    // layout2.c
    {"Set auto power off time", "设置自动关机时间"},
    {"Set new PIN", "设置新PIN码"},
    {"Set the PIN", "设置PIN码"},
    // menu.c
    {"Settings", "设置项"},
    {"Settings ", "设置"},
    // fsm_msg_common.h
    {"Settings applied", ""},
    // layout2.c layout2.c
    {"Shutdown", "自动关机"},
    // starcoin.c
    {"Sign at you own risk", "签名可能存在安全风险,请谨慎操作"},
    // layout2.c
    {"Sign binary message?", ""},
    // layout2.c
    {"Sign message?", "签名消息"},
    {"Signed by", "签名者"},
    {"Signed by:", "签名者:"},
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
    //
    {"Sleep Mode", "休眠模式"},
    // cosmos.c
    {"Source Address", "来源地址"},
    {"Source Coins", "来源金额"},
    // storage.c
    {"Starting up", "启动中"},
    // config.c
    {"Storage failure", ""},
    //
    {"Submit", "提交"},
    //
    {"Switch", "切换"},
    // ada.c
    {"The following transaction output contains tokens.",
     "此交易的交易输入中包含代币."},
    // protect.c
    {"The new PIN must be different from your wipe code.", ""},
    // recovery.c
    {"The seed is", "种子"},
    // recovery.c recovery.c
    {"The seed is INVALID!", "种子不可用"},
    {"The seed is valid", "种子可用"},
    // recovery.c
    {"The seed is valid and matches the one in the device",
     "种子可用并且与设备内的相同"},
    // recovery.c
    {"The seed is valid but does not match the one in the device",
     "种子可用但是与设备内的不相同"},
    //
    {"The seed phrases are the", " "},
    // protect.c
    {"The wipe code must be different from your PIN.", ""},
    // u2f.c
    {"This U2F device is", "此 U2F 设备"},
    //
    {"This cannot be undo!", "本操作不可撤销!"},
    // protect.c fsm_msg_common.h
    {"This firmware is incapable of passphrase entry on the device.", ""},
    // menu_list.c
    {"This will prevent you\nfrom using third-party\nwallet clients and\n"
     "websites which only\nsupport Trezor.",
     "这将会导致您无法使用一些仅支持 Trezor 的第三方客户端和网站."},
    // signing.c fsm_msg_coin.h
    {"Timestamp must be set.", ""},
    // signing.c fsm_msg_coin.h
    {"Timestamp not enabled on this coin.", ""},
    // cosmos.c
    {"Tip Amount", "小费金额"},
    {"Tipper", "小费支付方"},
    {"Title", "标题"},
    {"To", "接收方"},
    {"To learn more about\nhow to get NFT Pet,\ngo to the",
     "了解如何获取NFT Pet\n请前往众筹首页"},
    {"To learn more about\nhow to use, go to the\nHelp Center",
     "了解如何使用\n请前往帮助中心"},
    // ada.c
    {"Token Amount:", "代币金额:"},
    // ethereum.c
    {"Too much data", ""},
    // ada.c
    {"Total Amount", "总金额"},
    // startcoin.c
    {"Transaction data cannot be decoded", "交易无法解析"},
    // ethereum.c
    {"Transaction data:", "交易数据:"},
    // signing.c signing.c signing.c
    {"Transaction has changed during signing", ""},
    // fsm_msg_coin.h
    {"Transaction must have at least one input", ""},
    // fsm_msg_coin.h
    {"Transaction must have at least one output", ""},
    {"Transfer", "发送"},
    // menu_list.c
    {"Trezor Compat", "Trezor 兼容性"},
    {"Trezor Compatibility", "Trezor 兼容性"},
    // algo
    {"Txn type", "类型"},
    // ethereum.c
    {"Txtype out of bounds", ""},
    // cosmos.c
    {"Type", "类型"},
    {"Type:", "类型:"},
    // fsm_msg_common.h
    {"U2F counter set", ""},
    // layout2.c
    {"U2F security key?", "U2F安全密钥?"},
    // cosmos.c
    {"Undelegate", "取消委托"},
    // layout2.c
    {"Unencrypted hex data", ""},
    // layout2.c
    {"Unencrypted message", ""},
    // fsm.c
    {"Unexpected message", ""},
    // starcoin.c
    {"Unknown", "未知"},
    // layout2.c
    {"Unknown Mosaic", ""},
    // tron_ui.c
    {"Unknown Token", "未知代币"},
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
    {"Unrecognized", "未被识别"},
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
    //
    {"Use this passphrase?", "确认密语"},
    // cosmos
    {"Validator", "验证方"},
    {"Validator Destination", "验证方目标"},
    {"Validator Source", "验证方来源"},
    // fsm_msg_crypto.h
    {"Value length must be a multiple of 16", ""},
    // signing.c signing.c signing.c signing.c signing.c
    // signing.c fsm_msg_coin.h
    {"Value overflow", ""},

    // layout2.c
    {"Verified binary message", ""},
    // layout2.c
    {"Verified message", ""},
    {"Verify Address", "校验地址"},
    {"Verify All Words", "依次核对"},
    {"Verify PIN", "请输入原PIN码"},
    {"Verify Public Key", "校验公钥"},
    {"Verify Specified Word", "自由核对"},
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
//
#if ONEKEY_MINI

    {"Visiting Help Center \nand search \"Blind \nSigning\" to learn \n"
     "more\n \nhelp.onekey.so",
     "访问帮助中心搜索盲签\n了解更多\n \nhelp.onekey.so"},
#else
    {"Visiting Help Center and search \"Blind Signing\" to "
     "learn more\n help.onekey.so",
     "访问help.onekey.so搜索盲签了解更多"},
#endif
    // cosmos.c
    {"Vote", "投票"},
    {"Voter", "投票方"},
    {"WARNING: The device\nwill be reset after\n10 wrong attempts",
     "注意: 若累计输错10次\n设备将自动重置"},
    // config.c
    {"Waking up", "唤醒"},
    //
    {"Wallet created\nsuccessfully", "钱包创建成功"},
    //
    {"Wallet restored\nsuccessfully", "钱包恢复成功"},
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
    {"Word", "助记词"},
    {"Word not found in a wordlist", ""},
    //
    {"Write down", "请抄写助记词"},
    // layout2.c
    {"Write down your ", "请抄写你的"},
    //
    {"Wrong PIN for ", "您已输错"},
    // fsm_msg_coin.h
    {"Wrong address path", ""},
    // signing.c signing.c
    {"Wrong input script type", ""},
    {"Wrong mnemonic", "错误的助记词"},
    {"Wrong recovery phrase", "错误的助记词"},
    // recovery.c
    {"Wrong word retyped", ""},
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    {"Yes", "是"},
    //
    {"You still have 9 times", "连续输入10次将重置设备"},
    // recovery.c
    {"Your wallet has\nsuccessfully restored", "钱包恢复成功"},
    {"[Address]", "[地址]"},
    {"[Public Key]", "[公钥]"},
    {"[QR Code]", "[二维码]"},
    // layout2.c
    {"actions which might", "具有潜在风险的"},
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
    {"agree to trezor.io/tos", " "},
    // u2f.c
    {"already registered", "已在该应用程序"},
    // recovery.c
    {"and MATCHES", ""},
    // layout2.c
    {"and levy of", ""},
    // tron_ui.c
    {"and limit max fee to", "交易费上限"},
    // layout2.c c c
    {"and network fee of", ""},
    // fsm_msg_common.h
    {"answer to ping?", ""},
    //
    {"asset,Keep it safe", "凭证,请妥善保管"},
    // layout2.c
    {"auto-lock your device", ""},
    //
    {"backup of seed phrases", "助记词的物理备份"},
    // fsm_msg_common.h
    {"backup only", "仅备份"},
    // layout2.c
    {"be temporarily able", "暂时允许"},
    {"be unsafe?", "操作吗?"},
    //
    {"blockheight:", "区块高度:"},
    // bl_check.c
    {"broken.", ""},
    // recovery.c
    {"but does NOT MATCH", ""},
    //
    {"chances", "次机会"},
    // fsm_msg_common.h
    {"change bluetooth", ""},
    // fsm_msg_common.h
    {"change current PIN?", "修改PIN码"},
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
    // layout2.c
    {"checks?", "检查吗?"},
    // bl_check.c bl_check.c
    {"contact our support.", ""},
    // reset.c
    {"create a new wallet?", "创建新的钱包?"},
    // bl_check.c config.c
    {"detected.", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"device initialied success", ""},
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
    // layout2.c
    {"enforce strict safety", "执行严格的安全"},
    // lisk.c
    {"fee:", "手续费"},
    // ethereum.c
    {"for gas?", "手续费"},
    // fsm_msg_coin.h
    {"for selected coin.", ""},
    {"format", "格式"},
    // layout2.c
    {"from your wallet?", "从钱包中"},
    // startcoin.c
    {"from:", "从:"},
    // layout2.c
    {"hour", "小时"},
    //
    {"hours", "小时"},
    // recovery.c
    {"imported succeed", "导入成功"},
    // layout2.c
    {"in", ""},
    // layout2.c
    {"in raw units of", ""},
    // layout2.c
    {"in the same mosaic", ""},
    // u2f.c u2f.c
    {"in this application.", "中注册."},
    // layout2.c
    {"including fee:", "包含交易费:"},
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
    {"minute", "分钟"},
    //
    {"minutes", "分钟"},
    //
    {"mnemonic", "助记词"},
    // cosmos.c
    {"none", "无"},
    //
    {"number of Mnemonic", "助记词位数"},
    // recovery.c
    {"of your mnemonic", "助记词"},
    // recovery.c
    {"on your computer", "在电脑上"},
    //
    {"only way to recover your", "助记词是找回资产唯一"},
    // fsm_msg_coin.h
    {"own risk!", "风险"},
    // protect.c
    {"passphrase using", "密语"},
    // ethereum.c
    {"paying up to", "支付最多"},
    // fsm_msg_common.h fsm_msg_common.h
    {"protection?", "保护"},
    // layout2.c
    {"raw units of", ""},
    // fsm_msg_common.h
    {"reboot start", ""},
    // recovery.c
    {"recover the device?", "恢复钱包"},
    {"recovery phrase", "助记词"},
    // lisk.c
    {"register a delegate?", ""},
    // fsm_msg_common.h
    {"remove current PIN?", "移除当前PIN码?"},
    // fsm_msg_common.h
    {"restore data err", ""},
    //
    {"restore wallet", "恢复已拥有的钱包"},
    // layout2.c
    {"s", "秒"},
    // layout2.c protect.c
    {"second", "秒"},
    // fsm_msg_common.h
    {"seed import failed", ""},
    // fsm_msg_common.h
    {"seed import success", ""},
    //
    {"seed phrases", "助记词"},
    // signing.c
    {"segwit input amount", ""},
    // fsm_msg_common.h
    {"send entropy?", "发送熵值"},
    {"sender", "发送方"},
    // fsm_msg_common.h
    {"set a new wipe code?", ""},
    // fsm_msg_common.h
    {"set new PIN?", "设置PIN码"},
    // fsm_msg_common.h
    {"status always?", ""},
    // bl_check.c
    {"successfully", "成功"},
    // fsm_msg_common.h fsm_msg_common.h
    {"the U2F counter?", ""},
    // protect.c
    {"the computer's", "使用电脑的"},
    // recovery.c recovery.c
    {"the one in the device.", ""},
    //
    {"the passphrase!", "密语"},
    // layout2.c
    {"the same mosaic", ""},
    //
    {"times", "次"},
    //
    {"timestamp:", "时间戳:"},
    // layout2.c
    {"to", "到 "},
    {"to approve some", "执行一些"},
    //
    {"to back", "键返回"},
    {"to cancel", "键取消/返回"},
    {"to confirm", "键确认"},
    {"to confirm ", "键确认/下一步"},
    // protect.c
    {"to continue ...", " "},
    //
    {"to creat wallet", "开始创建新钱包"},
    // ethereum.c
    {"to new contract?", "新合约"},
    //
    {"to scroll down", "键下一页"},
    //
    {"to scroll up", "键上一页"},
    //
    {"to try", " "},
    // lisk.c
    {"to:", ""},
    // ethereum.c
    {"token", "代币"},
    //

    {"Transaction:", "交易:"},
    //
    {"transaction is set to", "被设定为"},
    // layout2.c layout2.c
    {"transaction?", "交易吗?"},
    {"user: ", "用户:"},
    // u2f.c
    {"was used to register", "已在该应用程序"},
    // fsm_msg_common.h
    {"wipe code?", "擦除PIN码"},
    // fsm_msg_common.h
    {"wipe the device?", "擦除设备"},
    //
    {"word", "单词"},
    //
    {"you still have ", "还有"}};

int LANGUAGE_ITEMS = sizeof(languages) / sizeof(languages[0]);
