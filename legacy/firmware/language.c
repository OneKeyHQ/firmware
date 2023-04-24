#include "language.h"

const char *languages[][2] = {
    //
    {" Disabled", "已禁用"},
    {" Enabled", "已启用"},
    {" Off", "已关闭"},
    //
    {" On", "已开启"},
    {" Public Key", " 公钥"},
    // protect.c
    {" attempts left, try again.", "尝试机会, 请重试"},
    {" attempts wrong, the device will be reset.", " 次, 设备将被重置."},
    // layout2.c
    {" decrypt for:", "解密"},
    {" is a non-standard path. Are you sure to use this path?",
     " 是非标准路径, 确定要继续使用该路径吗?"},
    // protect.c
    {" left ", "剩余"},
    // layout2.c
    {" login to:", "登录到"},
    // protect.c
    {" times", "次"},
    //
    {" to", "到"},
    // protect.c
    {" will be reset now...", "将立即重置..."},
    // reset.c
    {" words again.", " 个单词."},
    {" words called\nRecovery Phrase. Write it\ndown on sheet in order.",
     " 个单词\n, 称为助记词. 请按顺序将它\n们抄写在纸上."},
    {" words recovery phrase in order.", " 位的助记词."},
    // recovery.c
    {" words you just entered.", " 个单词."},
    {"##th", "第##个"},
    // menu_list.c
    {"(Custom)", "(自定义)"},
    // protect.c
    {"0 attempts left, device", "剩余 0 次尝试机会, 系统"},
    {"12 Words", "12 个单词"},
    {"18 Words", "18 个单词"},
    {"24 Words", "24 个单词"},
    {"9 attempts left, try again.", "剩余 9 次尝试机会, 请重试"},
    // starcoin.c
    {"APPROVE", "授权"},
    // fsm_msg_coin.h
    {"Abort", "取消"},
    // reset.c
    {"Abort Backup?", "终止备份?"},
    {"Abort Import?", "终止导入?"},
    // menu.c
    {"About Device", "关于设备"},
    // layout2.c
    {"Access hidden wallet?", "使用隐藏钱包?"},
    // ada.c
    {"Account:", "账号:"},
    // layout2.c
    {"Acitve", "激活"},
    // fsm_msg_coin.h fsm_msg_lisk.h
    {"Address:", "地址"},
    //
    {"Advance CFX Sign", "CFX高级签名"},
    //
    {"Advance ETH Sign", "ETH高级签名"},
    //
    {"Advance SOL Sign", "SOL高级签名"},
    // layout2.c
    {"Again", "再次"},
    // fsm_msg_common.h
    {"All data will be lost.", "所有数据将丢失"},
    // reset.c
    {"Almost Done!", "即将完成!"},
    // u2f.c
    {"Already registered.", "已注册"},
    {"Amount", "金额"},
    // u2f.c
    {"Another U2F device", "另外的U2F设备"},
    // layout2.c
    {"Any questions? Visit Help\nCenter for solutions:\nhelp.onekey.so",
     "还有其他问题? 请前往帮助\n中心寻求解答:\nhelp.onekey.so"},
    // reset.c
    {"Are you sure to abort this\nprocess? All progress\nwill be lost.",
     "确定要终止本次流程吗? \n所有的进度都将丢失."},
    //
    {"Are you sure to reset this \ndevice? This action can not be undo!",
     "您确定要重置设备吗? 该\n操作无法撤消!"},
    {"Are you sure to reset?", "确定要重置吗?"},
    // ada.c
    {"Asset Fingerprint:", "资产指纹:"},
    // u2f.c
    {"Authenticate", "认证"},
    // layout2.c
    {"Authenticity Check", "防伪检查"},
    // u2f.c
    {"Authorized by", "授权方"},
    // layout2.c
    {"Auto-Lock", "自动锁定"},  // msg
    // reset.c
    {"Awesome!", "棒!"},
    // layout2.c
    {"BACKUP FAILED!", "备份失败"},
    // layout2.c
    {"BLE enable:", "使用蓝牙:"},
    // layout2.c
    {"BLUETOOTH NAME:", "蓝牙名称:"},
    //
    {"BLUETOOTH VERSION:", "蓝牙版本:"},
    //
    {"BOOTLOADER:", "BOOTLOADER版本:"},
    // layout2.c
    {"BUILD ID:", "构建号:"},
    // recovery.c
    {"Back", "返回"},
    // layout2.c
    {"Back Up Recovery Phrase", "备份助记词"},
    {"Backup Only", "仅备份"},
    // protect.c
    {"Before start, verify your \ncurrent PIN.",
     "在开始之前, 请先验证当前 \nPIN 码."},
    // layout2.c layout2.c
    {"Bluetooth", "蓝牙"},
    // signing.c signing.c
    {"Burn tokens", "销毁代币"},
    // fsm.c
    {"Button expected", ""},  // msg
    // starcoin
    {"CANCEL", "取消"},
    // protect.c
    {"CAUTION!", "请注意!"},
    //
    {"CONFIRM SIGNING:", "确认签名:"},
    // fsm_msg_common.h
    {"Can't change se setting after device initialized", ""},  // msg
    // fsm_msg_coin.h
    // {"Can't encode address", ""},  // msg
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
    // menu_list.c
    {"Certification", "认证信息"},
    // cosmos.c
    {"Chain ID", "链 ID"},
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
    {"Check Path", "核对路径"},
    //
    {"Check Recovery Phrase", "核对助记词"},
    // reset.c
    {"Check Word ", "核对单词 "},
    // layout2.c
    {"Check Words Again", "再次检查助记词"},
    // recovery.c
    {"Check and try again.", "备份后重试."},
    //
    {"Check failed", "核对失败"},
    //
    {"Check passed", "核对通过"},
    //
    {"Check the entered", "请检查输入的"},
    //
    {"Check the seed", "检查助记词"},
    //
    {"Check this device with\nOneKey secure server?",
     "确定访问OneKey服务器对\n此设备进行防伪校验吗?"},
    // menu_list.c
    {"Check your Recovery \nPhrase backup, make sure \nit is exactly the same "
     "as \nthe one stored on device.",
     "检查你手中的备份, 确保与\n存储在设备上的助记词完全匹配."},
    // algorand.c
    {"Close to", "关闭账户并转移资产到"},
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
    // layout2.c
    {"Confirm Address", "确认地址"},
    {"Confirm OMNI Transaction:", "确认 OMNI 交易"},
    // layout2.c
    {"Confirm OP_RETURN:", "确认 OP_RETURN"},
    // ethereum.c
    {"Confirm fee", "确认交易费"},
    // layout2.c lisk.c
    {"Confirm sending", "确认发送"},
    // starcoin.c aptos.c
    {"Confirm signing", "确认签名"},
    // lisk.c lisk.c lisk.c lisk.c
    {"Confirm transaction", "确认交易"},
    // layout2.c layout2.c layout2.c
    {"Confirm transfer of", "确认转移"},
    //
    {"Confirm your operation!", "确认操作!"},
    // layout2.c
    {"Congratulations!", "恭喜!"},
    // layout2.c lisk.c reset.c fsm_msg_coin.h
    {"Continue", "继续"},
    // fsm_msg_coin.h
    {"Continue at your", "继续"},
    // fsm_msg_common.h
    {"Continue only if you", "继续"},
    // layout2.c
    {"Continue?", "继续"},
    {"Contract:", "合约:"},
    // recovery.c
    {"Correct!", "没问题!"},
    // layout2.c
    {"Create", "创建新钱包"},
    {"Create New Wallet", "创建新钱包"},
    {"Create account", "创建账户"},
    {"Create multisig", "创建多重签名"},
    // layout2.c
    {"DEVICE ID:", "设备编号:"},
    //
    {"DONE", "完成"},
    // fsm.c
    {"Data error", "数据错误"},
    // ethereum.c
    {"Data length exceeds limit", ""},  // msg
    // layout2.c
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
    // menu_list.c
    {"Device Info", "设备信息"},
    // reset.c reset.c
    {"Device has been reset", "设备已重置"},
    // protect.c
    {"Device reset complete, restart now!", "设备已重置, 请重启!"},
    {"Device reset in progress", "设备重置中"},
    // menu_list.c
    {"Disable", "禁用"},
    {"Disable Passphrase", "禁用 Passphrase"},
    {"Disable Trezor Compat", "禁用 Trezor 兼容"},
    {"Do not change this setting", "如果您不确定此操作的后"},
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
    {"Do you want to decrypt?", "请确认解密"},
    // fsm_msg_common.h
    {"Do you want to restart", "确定要重启设备并进入更新"},
    {"Do you want to set", "请确认设置"},
    // layout2.c
    {"Do you want to sign in?", "请确认登录"},
    {"Do you want to sign this\n", "确认要签署此 "},
    {"Do you want to verify this message?", "确认要验证此消息吗?"},
    //
    {"Done", "完成"},
    {"Done!", "完成!"},
    //
    {"Download OneKey Apps", "下载 OneKey 应用"},
    {"Download OneKey apps at:\nonekey.so/download",
     "下载 OneKey app 请前往:\nonekey.so/download"},
    //
    {"ETH advance signing turn", "ETH高级签名"},
    // menu_list.c
    {"Enable", "启用"},
    {"Enable Passphrase", "启用 Passphrase"},
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
    //
    {"Enter", "填入"},
    // protect.c
    {"Enter New PIN", "输入新 PIN 码"},
    {"Enter New PIN Again", "再次输入新 PIN 码"},
    {"Enter PIN", "输入 PIN 码"},
    //
    {"Enter Passphrase", "输入Passphrase"},
    {"Enter Recovery Phrase", "输入助记词"},
    // protect.c
    {"Enter new wipe code:", "输入新的擦除PIN码"},
    //
    {"Enter word ", "输入单词"},
    //
    {"Enter words to", "输入助记词"},
    //
    {"Enter your ", "请按顺序依次输入 "},
    {"Enter your 12-words  \nRecovery Phrase in order.",
     "请按顺序依次输入 12 位的\n助记词."},
    {"Enter your 18-words  \nRecovery Phrase in order.",
     "请按顺序依次输入 18 位的\n助记词."},
    {"Enter your 24-words  \nRecovery Phrase in order.",
     "请按顺序依次输入 24 位的\n助记词."},
    {"Enter your Passphrase on\nconnnected device.",
     "请在已连接的设备上输入\nPassphrase."},
    // menu_list.c
    {"Erase Device", "擦除设备"},
    // signing.c
    {"Error computing multisig fingerprint", ""},
    //
    {"Exit", "退出"},
    {"FIRMWARE VERSION:", "固件版本:"},
    // signing.c signing.c signing.c signing.c
    // signing.c
    // signing.c
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
    //
    {"Firmware will be erased!", "固件将被擦除!"},
    //
    {"Follow the prompts", "按照屏幕提示"},
    //
    {"Font:", "字体支持:"},
    //
    {"For more information", "了解更多"},
    // layout2.c
    {"Format:", "格式:"},
    {"From", "发送方"},
    // layout2.c
    {"GPG sign for:", ""},
    {"Gas Fee Cap", "燃料单价上限"},
    {"Gas Limit", "燃料用量上限"},
    {"Gas Premium", "优先费用"},
    // reset.c
    {"Generating a standard\nwallet with a new set of\nrecovery phrase.",
     "即将创建由一组新助记词生\n成的标准钱包."},
    // cosmos.c
    {"Granter", "授予人"},
    // layout2.c
    {"Guide", "使用教程"},
    // layout2.c c fsm_msg_common.h
    {"I take the risk", "确认风险"},
    // recovery.c
    {"INVALID!", "不可用"},
    // fsm_msg_common.h
    {"If import seed,", "如果导入种子"},
    // layout2.c
    {"Import Wallet", "导入钱包"},
    //
    {"Inconsistent PIN code", "PIN码前后不一致"},
    // protect.c
    {"Incorrect PIN", "PIN码错误"},
    // reset.c
    {"Incorrect word! check your", "单词不正确! 请再次检查您"},
    // protect.c
    {"Invalid Recovery Phrase. \nCheck your backup and try again.",
     "无效助记词.\n请检查你的备份后重试."},
    // recovery.c
    {"Invalid recovery phrase!", "无效的助记词!请检查您的"},
    //
    {"Invalid words", "助记词无效"},
    // recovery.c
    {"Invalid!", "无效!"},
    // menu_list.c
    {"It will take effect after \ndevice restart.", "将在设备重启后生效."},
    // layout2.c
    {"Label:", "设备名称:"},
    // layout2.c layout2.c
    {"Language", "语言"},
    // fsm_msg_common.h
    {"Loading private seed", "导入私钥"},
    // trezor.c
    {"Lock Device", "锁定设备"},
    //
    {"Locktime for this", "该交易时间锁定"},
    // layout2.c
    {"Login to:", "登录到"},
    //
    {"MODEL:", "型号:"},
    //
    {"Make sure you still have", "请确保您仍掌握"},
    {"Make sure you still have a backup of current wallet.",
     "继续该操作前, 请确保你仍持有当前钱包的助记词."},
    // layout2.c
    {"Maximum Fee Per Gas:", "交易费上限单价:"},
    {"Maximum Fee:", "交易费上限:"},
    // cosmos.c
    {"Memo", "备注"},
    // layout2.c
    {"Message", "消息"},
    // ethereum.c
    {"Message Hash", "消息哈希值"},
    //
    {"Mnemonic", "助记词"},
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
    //
    {"Next screen will show", "下一屏幕将显示"},
    {"Next, Follow the onscreen\ninstructions to set up your\nOneKey Classic.",
     "接下来,请按照屏幕上的指令\n完成开始设置您的 \nOneKey Classic"},
    {"Next, check the written ", "接下来, 请再次检查刚刚抄写\n的 "},
    {"Next, follow the guide and\ncheck words one by one.",
     "接下来, 请跟随引导, 逐一核\n对单词."},
    // layout2.c layout2.c layout2.c layout2.c layout2.c
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    // layout2.c c c c c c
    {"No", "否"},
    // layout2.c
    {"Not Actived", "未激活"},
    // protect.c
    {"Not Match!", "不匹配"},
    // algorand.c
    {"Note", "备注"},
    // layout2.c u2f.c u2f.c fsm_msg_common.h
    {"OK", "确认"},
    //
    {"Off", "关闭"},
    //
    {"Okay", "确认"},
    //
    {"On", "开启"},
    {"OneKey Classic is set up,\nyou will back to home\nscreen",
     "OneKey Classic 设置成功,\n即将回到系统主屏幕."},
    // cosmos.c
    {"Option", "选项"},
    // protect.c protect.c protect.c protect.c protect.c
    {"PIN Changed", "PIN 已更改"},
    // fsm_msg_common.h
    {"PIN code change", "PIN码修改"},
    // protect.c
    {"PIN code set", "PIN设置"},
    // fsm.c
    {"PIN expected", ""},
    {"PIN is set!", "PIN 码已设置!"},
    // fsm.c
    {"PIN mismatch", "两次输入不相同"},
    // protect.c
    {"PIN not match. Try again.", "前后输入的 PIN 码不一致, 请重试."},
    //
    {"Page down", "下一页"},
    //
    {"Page up", "上一页"},
    // layout2.c
    {"Path:", "路径:"},
    // protect.c protect.c
    {"Please confirm PIN", "请确认PIN码"},
    // recovery.c
    {"Please enter", "请输入"},
    // protect.c
    {"Please enter current PIN", "请输入当前PIN码"},
    // protect.c
    {"Please enter new PIN", "请输入新PIN码"},
    // recovery.c
    {"Please enter the", "请输入"},
    // protect.c
    {"Please enter your", "请输入你的"},
    // protect.c
    {"Please enter your PIN:", "请输入PIN码"},
    // layout2.c
    {"Please input PIN", "请输入PIN码"},
    // protect.c
    {"Please re-enter new PIN", "请再次输入新PIN码"},
    //
    {"Please reboot", "按确认将重启"},
    // recovery.c
    {"Please select the", "请选择"},
    //
    {"Please set the PIN", "请设置PIN码"},
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
    // ethereum.c
    {"Priority fee per gas:", "每份燃料消耗的优先费用"},
    // storage.c
    {"Processing...", "处理中..."},
    // cosmos.c
    {"Proposal", "提案"},
    {"Proposal ID", "提案 ID"},
    {"Propose", "投票"},
    {"Proposer", "提案申请人"},
    // lisk.c
    {"Public Key:", "公钥"},
    // layout2.c
    {"QR Code", "二维码"},
    //
    {"Quick Start", "快速上手"},
    {"Quit", "退出"},
    // layout2.c
    {"Quota:", "单次限额"},
    {"REJECT", "拒绝"},
    // ethereum.c layout2.c
    {"Really send", "确认发送"},
    {"Receiver", "接收方"},
    //
    {"Recipient", "接收方"},
    //
    {"Recovery Phrase ", "助记词"},
    {"Recovery Phrase is the \nonly way to restore the \nprivate keys that own "
     "the\nassets.",
     "助记词是找回私钥的唯一方式."},
    {"Recovery phrase is the \nonly way to recover your\nassets. So keep it in "
     "a\nsafe place.",
     "助记词是找回您加密资产的\n唯一方式, 请妥善保管."},
    {"Recovery phrase is valid\n but does not match.\nCheck and try again.",
     "输入的助记词有效但与设备\n中存储的不匹配.请检查您\n的备份后重试."},
    {"Recovery phrase verified pass", "助记词校验通过"},
    // cosmos.c
    {"Redelegate", "重新委托"},
    // u2f.c
    {"Register", "注册"},
    {"Reject", "拒绝"},
    // algorand.c
    {"Rekey to", "重新授权给"},
    // layout2.c
    {"Remaining times:", "剩余次数"},
    //
    {"Reset", "重置设备"},
    //
    {"Reset ", "重置"},
    // protect.c
    {"Reset Complete", "重置成功"},
    {"Reset Device", "重置设备"},
    {"Reset device to factory \ndefault.", "将设备还原为出厂设置."},
    //
    {"Restore", "恢复钱包"},
    {"Restore Trezor Compat", "恢复 Trezor 兼容模式"},
    {"Restore the wallet you\npreviously used from a \nrecovery phrase",
     "输入助记词来恢复您曾使用\n过的钱包."},
    //
    {"Retry", "重试"},
    // recovery.c
    {"Review Wordlist", "再次检查单词"},
    // layout2.c
    {"SE VERSION:", "SE版本:"},
    //
    {"SECURITY CHECK", "防伪校验"},
    // starcoin.c
    {"SENDER:", "发送方:"},
    //
    {"SERIAL NUMBER:", "序列号:"},
    //
    {"SOL advance signing turn", "SOL高级签名"},
    //
    {"Scan the QR code below", "扫描下方二维码"},
    //
    {"Security", "安全"},
    // protect.c
    {"Select Number of Word", "选择助记词位数"},
    //
    {"Select correct word below", "按提示选择正确的单词"},
    // ethereum.c
    {"Send", "发送"},
    {"Send SOL", "发送 SOL"},
    // layout2.c
    {"Send anyway?", ""},
    {"Send to:", "发送给:"},
    //
    {"Sender", "发送方"},
    // protect.c
    {"Set PIN", "设置 PIN 码"},
    // layout2.c
    {"Set Up", "开始设置"},
    // protect.c
    {"Set a 4 to 9-digits PIN to\nprotect your wallet.",
     "设置一个长度在 4 到 9 位的 \nPIN 码来保护你的钱包."},
    // layout2.c
    {"Set auto power off time", "设置自动关机时间"},
    // menu.c
    {"Settings", "系统设置"},
    // layout2.c layout2.c
    {"Shutdown", "关机"},
    {"Sign Binary Message?", "签署二进制消息?"},
    {"Sign Message", "签署消息"},
    {"Sign Transaction", "签署交易"},
    // starcoin
    {"Sign at you own risk", "签名可能存在安全风险,请谨慎操作"},
    // layout2.c
    {"Sign message?", "签名消息"},
    {"Signer:", "签名者"},
    // ethereum.c ethereum.c ethereum.c ethereum.c lisk.c
    {"Signing", "签名中"},
    // lisk.c signing.c signing.c signing.c signing.c
    // signing.c signing.c signing.c signing.c
    {"Signing transaction", "签名交易中"},
    // layout2.c
    {"Skip button confirm:", "跳过确认"},
    // layout2.c
    {"Skip pin check:", "免密支付"},
    //
    {"Sleep Mode", "休眠模式"},
    // cosmos.c
    {"Source Address", "来源地址"},
    {"Source Coins", "来源金额"},
    // ada.c
    {"Stake delegation", "质押委托"},
    {"Stake deregistration", "质押密钥取消注册"},
    {"Stake key registration", "质押密钥注册"},
    // storage.c
    {"Starting...", "启动中..."},
    //
    {"Submit", "提交"},
    // layout2.c
    {"Support", "用户支持"},
    //
    {"Switch", "切换"},
    {"Switch Input (Lowercase)", "切换输入法 (小写字母)"},
    {"Switch Input (Number)", "切换输入法 (数字)"},
    {"Switch Input (Symbol)", "切换输入法 (符号)"},
    {"Switch Input (Uppercase)", "切换输入法 (大写字母)"},
    // protect.c
    {"The device is reset,\nrestart now!", "设备已重置, 请重启!"},
    {"The following transaction output contains tokens.",
     "此交易的交易输入中包含代币."},
    // reset.c
    {"The next screen will start\ndisplay", "接下来, 屏幕将展示您刚刚输入的"},
    {"The next screen will start\ndisplay ", "接下来将依次展示 "},
    // recovery.c
    {"The seed is", "种子"},
    //
    {"This cannot be undo!", "本操作不可撤销!"},
    // protect.c fsm_msg_common.h
    {"This firmware is incapable of passphrase entry on the device.", ""},
    {"This will permanently \nerase all data stored on\nSecure Element (SE) "
     "and \ninternal storage,",
     "该操作将永久删除存储在内\n部存储或安全元件(SE)上的\n所有数据,"},
    {"This will prevent you from \nusing third-party wallet \nclients and "
     "websites which \nonly support Trezor.",
     "这将会导致您无法使用一些\n仅支持 Trezor 的第三方客户\n端和网站."},
    // cosmos.c
    {"Tip Amount", "小费金额"},
    {"Tipper", "小费支付方"},
    {"Title", "标题"},
    // signing.c fsm_msg_coin.h
    // ada.c
    {"To Pool:", "目标质押池:"},
    // layout2.c
    {"Token Amount:", "代币金额:"},
    {"Token Contract:", "Token 地址:"},
    // ethereum.c
    {"Total Amount:", "总金额:"},
    // tron_ui.c
    {"Transaction", "交易"},
    {"Transaction Fee", "交易费"},
    {"Transaction Type:", "交易类型"},
    // starcoin.c
    {"Transaction data cannot be decoded", "交易无法解析"},
    // ethereum.c
    {"Transaction data:", "交易数据:"},
    {"Transfer", "发送"},
    // menu_list.c
    {"Trezor Compat", "Trezor 兼容性"},
    {"Trezor Compatibility", "Trezor 兼容性"},
    // algo
    {"Txn type", "类型"},
    // cosmos.c
    {"Type", "类型"},
    // fsm_msg_ethereum.h
    {"Unable to show EIP-712 data. Sign at your own risk.",
     "无法显示 EIP-712 数据.\n请谨慎甄别项目方后决定是否签名, 自负风险"},
    // cosmos.c
    {"Undelegate", "取消委托"},
    // starcoin.c
    {"Unknown", "未知"},
    // layout2.c
    {"Unknown Mosaic", ""},
    // tron_ui.c
    {"Unknown Token", "未知代币"},
    // bl_check.c bl_check.c
    {"Unrecognized", "未被识别"},
    // config.c
    {"Updating", "更新"},
    // layout2.c
    {"Use SE:", "使用SE:"},
    // layout2.c
    {"Use this passphrase?", "使用此 Passphrase?"},
    // cosmos
    {"Validator", "验证方"},
    {"Validator Destination", "验证方目标"},
    {"Validator Source", "验证方来源"},
    // layout2.c
    {"Verified binary message?", "验证二进制信息"},
    // layout2.c
    {"Verified message", "验证信息"},
    // storage.c
    {"Verifing...", "验证中..."},
    // layout2.c
    {"View Data", "查看数据"},
    // cosmos.c
    {"Vote", "投票"},
    {"Voter", "投票方"},
    // menu_list.c
    {"WARNING", "警告"},
    {"WARNING! (1/2)", "警告! (1/2)"},
    {"WARNING! (2/2)", "警告! (2/2)"},
    // config.c
    {"Waking up", "唤醒"},
    //
    {"Wallet Recovery Success", "钱包恢复成功"},
    //
    {"Wallet created success", "钱包创建成功"},
    {"Wallet is ready! Download\nOneKey apps and have fun\nwith your OneKey "
     "Classic.",
     "钱包就绪!快去下载OneKey\n应用程序并正式开始使用\n您的 OneKey Classic "
     "吧."},
    {"Want to check authenticity\nof this device? Go to the \nwebsite below "
     "for help:\nonekey.so/auth",
     "想检测设备的真伪? 请前往\n下方网址寻求帮助:\nonekey.so/auth"},
    // layout2.c
    {"Warning!", "警告"},
    // layout2.c
    {"Welcome to OneKey!", "感谢使用 OneKey!"},
    // cosmos.c
    {"Withdraw Reward", "提取奖励"},
    {"Withdraw Validator Commission", "提取验证者佣金"},
    // recovery.c
    {"Word not found in a wordlist", ""},
    {"Wordlist ", "单词列表 "},
    //
    {"Word", "单词"},
    //
    {"Write down", "请抄写助记词"},
    // layout2.c
    {"Write down your ", "请抄写你的"},
    //
    {"Wrong PIN for ", "您已输错"},
    //
    {"Wrong recovery phrase", "错误的助记词"},
    // recovery.c
    {"Wrong word retyped", ""},
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    {"Yes", "是"},
    //
    {"You still have 9 times", "连续输入10次将重置设备"},
    {"You wallet is restored.", "钱包恢复成功."},
    // reset.c
    {"Your backup is complete.", "助记词备份成功."},
    // layout2.c
    {"active device", "激活设备"},
    // layout2.c
    {"after ", "再输错 "},
    // u2f.c
    {"already registered", "已注册"},
    // tron_ui.c
    {"and limit max fee to", "交易费上限"},
    // reset.c
    {"backup and try again.", "的备份后重试."},
    //
    {"backup of words", "助记词的物理备份"},
    // fsm_msg_common.h
    {"backup only", "仅备份"},
    //
    {"blockheight:", "区块高度:"},
    // recovery.c
    {"but does NOT MATCH", ""},
    //
    {"chances", "次机会"},
    // fsm_msg_common.h
    {"change current PIN?", "修改PIN码"},
    // fsm_msg_common.h
    {"change fastpay settings", "修改快捷支付"},
    // fsm_msg_common.h
    {"change language to", "修改语言为"},
    // fsm_msg_common.h
    {"change name to", "修改名称为"},
    // fsm_msg_common.h
    {"change the home screen", "修改屏保"},
    // signing.c
    {"changed", "修改"},
    // reset.c
    {"create a new wallet?", "创建新的钱包?"},
    // tron_ui.c
    {"data", "数据"},
    // fsm_msg_common.h
    {"device in update mode?", "模式吗?"},
    {"device is used for", "设备用于"},
    // fsm_msg_common.h
    {"disable passphrase", "禁用密语"},
    // fsm_msg_common.h fsm_msg_common.h
    {"doing!", "操作"},
    // fsm_msg_common.h
    {"enable passphrase", "使用密语"},
    // lisk.c
    {"fee:", "手续费"},
    // ethereum.c
    {"for gas?", "手续费"},
    // fsm_msg_coin.h
    {"format", "格式"},
    // layout2.c
    {"from your wallet?", "从钱包中"},
    // startcoin.c
    {"from:", "从:"},
    // layout2.c
    {"hour", "小时"},
    //
    {"hours", "小时"},
    // menu_list.c
    {"if you not sure.", "果, 请勿更改此设置"},
    // recovery.c
    {"imported succeed", "导入成功"},
    // layout2.c
    {"including private keys and settings.", "包括私钥和当前\n设置."},
    // layout2.c
    {"is unexpectedly high.", "过高"},
    {"key to continue", "键以继续"},
    {"key to go back", "键回到上一页"},
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
    {"number of recovery phrase", "助记词位数"},
    // recovery.c
    {"of your recovery phrase", "助记词"},
    // recovery.c
    {"on your computer", "在电脑上"},
    // fsm_msg_coin.h
    {"own risk!", "风险"},
    // protect.c
    {"passphrase using", "密语"},
    // ethereum.c
    {"paying up to", "支付最多"},
    // fsm_msg_common.h fsm_msg_common.h
    {"protection?", "保护"},
    // recovery.c
    {"recover the device?", "恢复钱包"},
    {"restore wallet", "恢复已拥有的钱包"},
    // layout2.c
    {"s", "秒"},
    // layout2.c protect.c
    {"second", "秒"},
    // fsm_msg_common.h
    {"send entropy?", "发送熵值"},
    {"sender", "发送方"},
    // fsm_msg_common.h
    {"set new PIN?", "设置PIN码"},
    // bl_check.c
    {"successfully", "成功"},
    // protect.c
    {"the computer's", "使用电脑的"},
    // reset.c
    {"the only way to recover", "助记词是找回资产唯一"},
    //
    {"the passphrase!", "密语"},
    //
    {"times", "次"},
    //
    {"timestamp:", "时间戳:"},
    //
    {"to back", "键返回"},
    //
    {"to confirm", "键确认"},
    //
    {"to creat wallet", "开始创建新钱包"},
    // ethereum.c
    {"to new contract?", "新合约"},
    //
    {"to scroll down", "键查看下一页"},
    //
    {"to scroll up", "键查看上一页"},
    //
    // ethereum.c
    {"token", "代币"},
    //
    {"transaction is set to", "被设定为"},
    // layout2.c layout2.c
    {"transaction?", "交易?"},
    {"user: ", "用户:"},
    // u2f.c
    {"was used to register", "注册"},
    // fsm_msg_common.h
    {"wipe code?", "擦除PIN码"},
    // fsm_msg_common.h
    {"wipe the device?", "擦除设备"},
    //
    {"word", "助记词"},
    //
    {"words", "助记词"},
    //
    {"you still have ", "还有"},
    // reset.c
    {"your asset,Keep it safe", "凭证,请妥善保管"},
};

int LANGUAGE_ITEMS = sizeof(languages) / sizeof(languages[0]);
