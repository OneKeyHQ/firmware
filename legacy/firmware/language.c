#include "language.h"

const char *languages[][2] = {
    //
    {" Off", "已關閉"},
    //
    {" On", "已開啟"},
    // layout2.c
    {" decrypt for:", "解密"},
    // layout2.c
    {" login to:", "登錄到"},
    //
    {" to", "到"},
    // recovery.c
    {"##th", "第##個"},
    // starcoin.c
    {"APPROVE", "授權"},
    // fsm_msg_coin.h
    {"Abort", "取消"},
    // menu.c
    {"About", "關於本機"},
    // layout2.c
    {"Absolute Levy", ""},
    //
    {"Access hidden wallet?", "使用隱藏錢包?"},
    // layout2.c
    {"Acitve", "激活"},
    // fsm.c
    {"Action cancelled by user", ""},  // msg
    // fsm_msg_coin.h fsm_msg_lisk.h
    {"Address:", "地址"},
    //
    {"Advance CFX Sign", "CFX高級簽名"},
    //
    {"Advance ETH Sign", "ETH高級簽名"},
    //
    {"Advance SOL Sign", "SOL高級簽名"},
    //
    {"After enabling \n\"Blind Signing\",your device  will support signing for "
     "messages and transactions, but it can't decode the metadata.",
     "\n開啟盲簽後,設備將支持對消息或交易進行簽名,但不會解析元數據"},
    // layout2.c
    {"Again", "再次"},
    // fsm_msg_common.h
    {"All data will be lost.", "所有數據將丟失"},
    // u2f.c
    {"Already registered.", "已註冊"},
    {"Amount", "金額"},
    // u2f.c
    {"Another U2F device", "另外的U2F設備"},
    //
    {"Are you sure to reset?", "確定要重置嗎?"},
    //
    {"Are you sure to update?", "確定升級固件嗎?"},
    // u2f.c
    {"Authenticate", "認證"},
    {"Authorized by", "授權方"},
    // fsm_msg_common.h
    {"Auto-lock delay too long", ""},  // msg
    // fsm_msg_common.h
    {"Auto-lock delay too short", ""},  // msg
    //
    {"AutoLock", "鎖定時間"},
    // layout2.c
    {"BACKUP FAILED!", "備份失敗"},
    // layout2.c
    {"BLE enable:", "使用藍牙:"},
    // layout2.c
    {"BLUETOOTH NAME:", "藍牙名稱:"},
    //
    {"BLUETOOTH VERSION:", "藍牙版本:"},
    //
    {"BOOTLOADER:", "BOOTLOADER版本:"},
    // layout2.c
    {"BUILD ID:", "構建號:"},
    // recovery.c
    {"Back", "返回"},
    // layout2.c
    {"Backup Only", "僅備份"},
    //
    {"Before your start", "繼續操作前"},
    //
    {"Blind Signing", "盲簽"},
    // layout2.c layout2.c
    {"Bluetooth", "藍牙"},
    // signing.c
    {"Both address and address_n provided.", ""},  // msg
    // signing.c
    {"Branch ID must be set.", ""},  // msg
    // signing.c signing.c
    {"Branch ID not enabled on this coin.", ""},  // msg
    {"Burn tokens", "銷毀代幣"},
    // fsm.c
    {"Button expected", ""},  // msg
    // recovery.c reset.c
    {"By continuing you", " "},
    // starcoin
    {"CANCEL", "取消"},
    {"CONFIRM SIGNING:", "確認簽名:"},
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
    // cosmos.c
    {"Chain ID", "鏈 ID"},
    // ethereum.c
    {"Chain Id out of bounds", ""},  // msg
    // cosmos.c
    {"Chain Name", "鏈名稱"},
    //
    {"Change PIN", "修改PIN碼"},
    // layout2.c
    {"Change language to", "設置語言為"},
    //
    {"Check Mnemonic", "查看助記詞"},
    //
    {"Check PIN Code", "請先校驗設備原PIN碼"},
    //
    {"Check Recovery Phrase", "助記詞核對"},
    //
    {"Check failed", "核對失敗"},
    //
    {"Check passed", "核對通過"},
    //
    {"Check the entered", "請檢查輸入的"},
    //
    {"Check the seed", "檢查助記詞"},
    // layout2.c
    {"Check the written ", "請檢查寫下的"},
    //
    {"Check this device with\nOneKey secure server?",
     "確定訪問OneKey服務器對\n此設備進行防偽校驗嗎?"},
    // algorand.c
    {"Close to", "關閉賬戶並轉移資產到"},
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
    {"Confirm", "確認"},
    // layout2.c
    {"Confirm OMNI Transaction:", "確認 OMNI 交易"},
    // layout2.c
    {"Confirm OP_RETURN:", "確認 OP_RETURN"},
    // layout2.c lisk.c
    {"Confirm address?", "確認地址"},
    // ethereum.c
    {"Confirm fee", "確認交易費"},
    // layout2.c lisk.c
    {"Confirm sending", "確認發送"},
    // starcoin.c aptos.c
    {"Confirm signing", "確認簽名"},
    // lisk.c lisk.c lisk.c lisk.c
    {"Confirm transaction", "確認交易"},
    // layout2.c layout2.c layout2.c
    {"Confirm transfer of", "確認轉移"},
    //
    {"Confirm your operation!", "確認操作!"},
    // layout2.c lisk.c reset.c fsm_msg_coin.h
    {"Continue", "繼續"},
    // fsm_msg_coin.h
    {"Continue at your", "繼續"},
    // fsm_msg_common.h
    {"Continue only if you", "繼續"},
    // layout2.c
    {"Continue?", "繼續"},
    // layout2.c
    {"Create", "創建新錢包"},
    {"Create account", "創建賬戶"},
    {"Create multisig", "創建多重簽名"},
    // layout2.c
    {"DEVICE ID:", "設備編號:"},
    //
    {"DONE", "完成"},
    // fsm.c
    {"Data error", "數據錯誤"},
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
    {"Decrypt value of this key?", "為該值解密"},
    // layout2.c
    {"Decrypted message", "解密消息"},
    // layout2.c
    {"Decrypted signed message", "解密簽名消息"},
    // cosmos.c
    {"Delegate", "委托"},
    {"Delegator", "代理方"},
    {"Deposit", "存款"},
    {"Deposit Amount", "存款金額"},
    {"Deposit Denomination", "存款面值"},
    {"Depositer", "存款方"},
    {"Description", "描述"},
    {"Destination Address", "目標地址"},
    {"Destination Coins", "來源金額"},
    // reset.c reset.c
    {"Device failed initialized", ""},  // msg
    //
    {"Device has been reset", "設備已重置"},
    // fsm.c
    {"Device is already initialized. Use Wipe first.", ""},  // msg
    // fsm_msg_common.h fsm_msg_common.h
    {"Device loaded", ""},  // msg
    // config.c fsm.c
    {"Device not initialized", ""},  // msg
    // recovery.c
    {"Device recovered", ""},  // msg
    //
    {"Device reset in progress", "設備重置中"},
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
    {"Do you really want to", "請確認"},
    //
    {"Do you really want to \ndisable passphrase protection?",
     "要禁用passphrase加密嗎?"},
    //
    {"Do you really want to \nenable passphrase protection?",
     "要啟用passphrase加密嗎?"},
    // fsm_msg_common.h
    {"Do you want to", "請確認"},
    // layout2.c
    {"Do you want to decrypt?", "請確認解密"},
    // fsm_msg_common.h
    {"Do you want to set", "請確認設置"},
    // layout2.c
    {"Do you want to sign in?", "請確認登錄"},
    //
    {"Done", "完成"},
    //
    {"Download Onekey", "下載Onekey"},
    //
    {"ETH advance signing turn", "ETH高級簽名"},
    // ethereum.c
    {"Empty data chunk received", ""},  // msg
    // signing.c signing.c
    {"Encountered invalid prevhash", ""},  // msg
    // layout2.c
    {"Encrypt message?", "加密消息"},
    // layout2.c
    {"Encrypt value of this key?", "為這個值加密"},
    // layout2.c
    {"Encrypt+Sign message?", "加密+簽名消息"},
    // layout2.c
    {"Encrypted hex data", "加密十六進製數據"},
    // layout2.c
    {"Encrypted message", "加密信息"},
    // layout2.c layout2.c
    {"English", "簡體中文"},
    //
    {"Enter", "填入"},
    //
    {"Enter PIN to unlock", "輸入PIN碼解鎖"},
    //
    {"Enter Passphrase", "輸入Passphrase"},
    // protect.c
    {"Enter new wipe code:", "輸入新的擦除PIN碼"},
    //
    {"Enter word ", "輸入助記詞"},
    //
    {"Enter words to", "輸入助記詞"},
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
    //
    {"FIRMWARE VERSION:", "固件版本:"},
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
    {"Fee", "手續費"},
    // layout2.c
    {"Fee included:", "已包含手續費"},
    {"Fee payer", "手續費支付方"},
    // layout2.c
    {"Finish", "完成"},
    // fsm.c
    {"Firmware error", "固件錯誤"},
    // layout2.c
    {"Firmware version:", "固件版本:"},
    //
    {"Firmware will be erased!", "固件將被擦除!"},
    // fsm_msg_common.h
    {"Flags applied", ""},  // msg
    //
    {"Follow the prompts", "按照屏幕提示"},
    //
    {"Font:", "字體支持:"},
    //
    {"For more information", "了解更多"},
    // fsm_msg_common.h
    {"Forbidden field set in dry-run", ""},  // msg
    {"From", "發送方"},
    // layout2.c
    {"GPG sign for:", ""},
    // cosmos.c
    {"Granter", "授予人"},
    // layout2.c
    {"Guide", "使用教程"},
    // layout2.c c fsm_msg_common.h
    {"I take the risk", "確認風險"},
    // recovery.c
    {"INVALID!", "不可用"},
    // fsm_msg_common.h
    {"If import seed,", "如果導入種子"},
    //
    {"Inconsistent PIN code", "PIN碼前後不一致"},
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
    //
    {"Invalid words", "助記詞無效"},
    // layout2.c
    {"Label:", "設備名稱:"},
    // layout2.c layout2.c
    {"Language", "語言"},
    // layout2.c
    {"Levy is", ""},
    // fsm_msg_common.h
    {"Loading private seed", "導入私鑰"},
    // trezor.c
    {"Lock Device", "鎖定設備"},
    //
    {"Locktime for this", "該交易時間鎖定"},
    // layout2.c
    {"Login to:", "登錄到"},
    //
    {"MODEL:", "型號:"},
    //
    {"Make sure you still have", "請確保您仍掌握"},
    // ethereum.c
    {"Malformed address", ""},
    // ethereum.c
    {"Malformed signature", ""},
    {"Maximum fee", "交易費上限"},
    {"Maximum fee per gas", "交易費上限單價"},
    // cosmos.c
    {"Memo", "備註"},
    // ethereum.c
    {"Message Hash", "消息哈希值"},
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
    {"Mnemonic", "助記詞"},
    // fsm_msg_common.h fsm_msg_common.h
    {"Mnemonic with wrong checksum provided", ""},
    // layout2.c
    {"Mosaic", ""},
    // layout2.c
    {"Mosaic Description", ""},
    // cosmos.c
    {"Multi Send", "多簽"},
    // signing.c signing.c
    {"Multisig field provided but not expected.", ""},
    {"NEXT", "繼續"},
    // layout2.c
    {"Need Backup", "未備份"},
    //
    {"Never", "永不"},
    // layout2.c
    {"Next", "繼續"},
    //
    {"Next screen will show", "下一屏幕將顯示"},
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
    // algorand.c
    {"Note", "備註"},
    // layout2.c u2f.c u2f.c fsm_msg_common.h
    {"OK", "確認"},
    // signing.c
    {"OP RETURN data provided but not OP RETURN script type.", ""},
    // signing.c
    {"OP_RETURN output with address or multisig", ""},
    // signing.c
    {"OP_RETURN output with non-zero amount", ""},
    //
    {"Off", "關閉"},
    //
    {"Okay", "確認"},
    //
    {"On", "開啟"},
    // cosmos.c
    {"Option", "選項"},
    // signing.c
    {"Output's address_n provided but not expected.", ""},
    // protect.c protect.c protect.c protect.c protect.c
    // fsm.c
    {"PIN cancelled", ""},
    // fsm_msg_common.h
    {"PIN code change", "PIN碼修改"},
    //
    {"PIN code set", "PIN設置"},
    // fsm.c
    {"PIN expected", ""},
    // fsm.c
    {"PIN invalid", "PIN碼錯誤"},
    // fsm.c
    {"PIN mismatch", "兩次輸入不相同"},
    // fsm_msg_common.h fsm_msg_common.h
    {"PIN removed", ""},
    //
    {"Page down", "下一頁"},
    //
    {"Page up", "上一頁"},
    // layout2.c
    {"Path: m", ""},
    // layout2.c
    {"Percentile Levy", ""},
    // protect.c protect.c
    {"Please confirm PIN", "請確認PIN碼"},
    // recovery.c
    {"Please enter", "請輸入"},
    //
    {"Please enter 12 words\nin order to verify\nyour recovery phrase",
     "請依次輸入12位助記詞進\n行核對"},
    {"Please enter 18 words\nin order to verify\nyour recovery phrase",
     "請依次輸入18位助記詞進\n行核對"},
    {"Please enter 24 words\nin order to verify\nyour recovery phrase",
     "請依次輸入24位助記詞進\n行核對"},
    // protect.c
    {"Please enter current PIN", "請輸入當前PIN碼"},
    // protect.c
    {"Please enter new PIN", "請輸入新PIN碼"},
    // recovery.c
    {"Please enter the", "請輸入"},
    // protect.c
    {"Please enter your", "請輸入你的"},
    // protect.c
    {"Please enter your PIN:", "請輸入PIN碼"},
    // layout2.c
    {"Please input PIN", "請輸入PIN碼"},
    // protect.c
    {"Please re-enter new PIN", "請再次輸入新PIN碼"},
    //
    {"Please reboot", "按確認將重啟"},
    // bl_check.c
    {"Please reconnect", ""},
    // recovery.c
    {"Please select the", "請選擇"},
    //
    {"Please set the PIN", "請設置PIN碼"},
    //
    {"Please try again", "請重新輸入"},
    // protect.c
    {"Please wait", "請等待"},
    //
    {"Press", "按"},
    //
    {"Press any key to continue", "按任意鍵繼續"},
    //
    {"Prev", "後退"},
    // ethereum.c
    {"Priority fee per gas", "每份燃料消耗的優先費用"},
    // fsm.c
    {"Process error", ""},
    // storage.c
    {"Processing", "處理中"},
    // cosmos.c
    {"Proposal", "提案"},
    {"Proposal ID", "提案 ID"},
    {"Propose", "投票"},
    {"Proposer", "提案申請人"},
    // signing.c
    {"Pubkey not found in multisig script", ""},
    // lisk.c
    {"Public Key:", "公鑰"},
    // layout2.c
    {"QR Code", "二維碼"},
    //
    {"Quit", "退出"},
    // layout2.c
    {"Quota:", "單次限額"},
    {"REJECT", "拒絕"},
    // layout2.c
    {"Raw levy value is", ""},
    // protect.c
    {"Re-enter new wipe code:", ""},
    // ethereum.c layout2.c
    {"Really send", "確認發送"},
    {"Receiver", "接收方"},
    //
    {"Recipient", "接收方"},
    //
    {"Recovery phrase verified pass", "助記詞校驗通過"},
    // cosmos.c
    {"Redelegate", "重新委托"},
    // u2f.c
    {"Register", "註冊"},
    // algorand.c
    {"Rekey to", "重新授權給"},
    // layout2.c
    {"Remaining times:", "剩余次數"},
    //
    {"Reset", "重置設備"},
    //
    {"Reset ", "重置"},
    //
    {"Restore", "恢復錢包"},
    //
    {"Retry", "重試"},
    // layout2.c
    {"SE VERSION:", "SE版本:"},
    //
    {"SECURITY CHECK", "防偽校驗"},
    // layout2.c
    {"SEEDLESS", ""},
    // starcoin.c
    {"SENDER:", "發送方:"},
    //
    {"SERIAL NUMBER:", "序列號:"},
    //
    {"SOL advance signing turn", "SOL高級簽名"},
    // ethereum.c
    {"Safety check failed", ""},
    //
    {"Scan the QR code below", "掃描下方二維碼"},
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
    //
    {"Select correct word below", "按提示選擇正確的單詞"},
    //
    {"Select your", "選擇你的"},
    // ethereum.c
    {"Send", "發送"},
    {"Send SOL", "發送 SOL"},
    // layout2.c
    {"Send anyway?", ""},
    //
    {"Sender", "發送方"},
    // fsm_msg_common.h
    {"Session cleared", ""},
    // layout2.c
    {"Set auto power off time", "設置自動關機時間"},
    // menu.c
    {"Settings", "設置項"},
    // fsm_msg_common.h
    {"Settings applied", ""},
    // layout2.c layout2.c
    {"Shutdown", "自動關機"},
    // starcoin
    {"Sign at you own risk", "簽名可能存在安全風險,請謹慎操作"},
    // layout2.c
    {"Sign binary message?", ""},
    // layout2.c
    {"Sign message?", "簽名消息"},
    // ethereum.c ethereum.c ethereum.c ethereum.c lisk.c
    {"Signing", "簽名中"},
    // signing.c
    {"Signing error", ""},
    // ethereum.c ethereum.c signing.c
    {"Signing failed", ""},
    // lisk.c signing.c signing.c signing.c signing.c
    // signing.c signing.c signing.c signing.c
    {"Signing transaction", "簽名交易中"},
    // layout2.c
    {"Simple send of ", ""},
    // fsm_msg_coin.h fsm_msg_crypto.h
    {"Singing", ""},
    // layout2.c
    {"Skip button confirm:", "跳過確認"},
    // layout2.c
    {"Skip pin check:", "免密支付"},
    //
    {"Sleep Mode", "休眠模式"},
    // cosmos.c
    {"Source Address", "來源地址"},
    {"Source Coins", "來源金額"},
    // storage.c
    {"Starting up", "啟動中"},
    // config.c
    {"Storage failure", ""},
    //
    {"Submit", "提交"},
    //
    {"Switch", "切換"},
    // protect.c
    {"The new PIN must be different from your wipe code.", ""},
    //
    {"The recovery phrase are", " "},
    // recovery.c
    {"The seed is", "種子"},
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
    //
    {"This cannot be undo!", "本操作不可撤銷!"},
    // protect.c fsm_msg_common.h
    {"This firmware is incapable of passphrase entry on the device.", ""},
    // signing.c fsm_msg_coin.h
    {"Timestamp must be set.", ""},
    // signing.c fsm_msg_coin.h
    {"Timestamp not enabled on this coin.", ""},
    // cosmos.c
    {"Tip Amount", "小費金額"},
    {"Tipper", "小費支付方"},
    {"Title", "標題"},
    // signing.c fsm_msg_coin.h
    {"To", "接收方"},
    // ethereum.c
    {"Too much data", ""},
    // tron_ui.c
    {"Transaction", "交易"},
    // starcoin.c
    {"Transaction data cannot be decoded", "交易無法解析"},
    // ethereum.c
    {"Transaction data:", "交易數據:"},
    // signing.c signing.c signing.c
    {"Transaction has changed during signing", ""},
    // fsm_msg_coin.h
    {"Transaction must have at least one input", ""},
    // fsm_msg_coin.h
    {"Transaction must have at least one output", ""},
    {"Transfer", "發送"},
    // algo
    {"Txn type", "類型"},
    // ethereum.c
    {"Txtype out of bounds", ""},
    // cosmos.c
    {"Type", "類型"},
    // fsm_msg_common.h
    {"U2F counter set", ""},
    // layout2.c
    {"U2F security key?", ""},
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
    {"Unknown Token", "未知代幣"},
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
    {"Unrecognized", "未被識別"},
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
    {"Use this passphrase?", "確認密語"},
    // cosmos
    {"Validator", "驗證方"},
    {"Validator Destination", "驗證方目標"},
    {"Validator Source", "驗證方來源"},
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
    {"Verifying PIN", "校驗PIN"},
    // signing.c
    {"Version group ID must be set when version >= 3.", ""},
    // signing.c
    {"Version group ID must be set.", ""},
    // signing.c signing.c
    {"Version group ID not enabled on this coin.", ""},
    //
    {"Visiting Help Center and search \"Blind Signing\" to "
     "learn more\n help.onekey.so",
     "訪問help.onekey.so搜索盲簽了解更多"},
    // cosmos.c
    {"Vote", "投票"},
    {"Voter", "投票方"},
    // config.c
    {"Waking up", "喚醒"},
    //
    {"Wallet Recovery Success", "錢包恢復成功"},
    //
    {"Wallet created success", "錢包創建成功"},
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
    // cosmos.c
    {"Withdraw Reward", "提取獎勵"},
    {"Withdraw Validator Commission", "提取驗證者傭金"},
    // recovery.c
    {"Word not found in a wordlist", ""},
    //
    {"Words", "助記詞"},
    //
    {"Write down", "請抄寫助記詞"},
    // layout2.c
    {"Write down your ", "請抄寫你的"},
    //
    {"Wrong PIN for ", "您已輸錯"},
    // fsm_msg_coin.h
    {"Wrong address path", ""},
    // signing.c signing.c
    {"Wrong input script type", ""},
    //
    {"Wrong recovery phrase", "錯誤的助記詞"},
    // recovery.c
    {"Wrong word retyped", ""},
    // layout2.c layout2.c layout2.c layout2.c
    // layout2.c
    {"Yes", "是"},
    //
    {"You still have 9 times", "連續輸入10次將重置設備"},
    // layout2.c
    {"active device", "激活設備"},
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
    {"already registered", "已註冊"},
    // recovery.c
    {"and MATCHES", ""},
    // layout2.c
    {"and levy of", ""},
    // tron_ui.c
    {"and limit max fee to", "交易費上限"},
    // layout2.c c c
    {"and network fee of", ""},
    // fsm_msg_common.h
    {"answer to ping?", ""},
    // layout2.c
    {"auto-lock your device", ""},
    //
    {"backup of words", "助記詞的物理備份"},
    // fsm_msg_common.h
    {"backup only", "僅備份"},
    //
    {"blockheight:", "區塊高度:"},
    // bl_check.c
    {"broken.", ""},
    // recovery.c
    {"but does NOT MATCH", ""},
    //
    {"chances", "次機會"},
    // fsm_msg_common.h
    {"change bluetooth", ""},
    // fsm_msg_common.h
    {"change current PIN?", "修改PIN碼"},
    // fsm_msg_common.h
    {"change fastpay settings", "修改快捷支付"},
    // fsm_msg_common.h
    {"change language to", "修改語言為"},
    // fsm_msg_common.h
    {"change name to", "修改名稱為"},
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
    {"create a new wallet?", "創建新的錢包?"},
    // tron_ui.c
    {"data", "數據"},
    // bl_check.c config.c
    {"detected.", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"device initialied success", ""},
    // fsm_msg_common.h
    {"device is used for", "設備用於"},
    // fsm_msg_common.h
    {"disable passphrase", "禁用密語"},
    // fsm_msg_common.h
    {"disable wipe code", ""},
    // fsm_msg_common.h fsm_msg_common.h
    {"doing!", "操作"},
    // fsm_msg_common.h
    {"enable passphrase", "使用密語"},
    // lisk.c
    {"fee:", "手續費"},
    // ethereum.c
    {"for gas?", "手續費"},
    // fsm_msg_coin.h
    {"for selected coin.", ""},
    {"format", "格式"},
    // layout2.c
    {"from your wallet?", "從錢包中"},
    // startcoin.c
    {"from:", "從:"},
    // layout2.c
    {"hour", "小時"},
    //
    {"hours", "小時"},
    // recovery.c
    {"imported succeed", "導入成功"},
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
    {"is unexpectedly high.", "過高"},
    // protect.c
    {"keyboard.", "鍵盤"},
    // fsm_msg_common.h fsm_msg_common.h
    {"know what you are", "清楚你的"},
    // trezor.c
    {"lock your Trezor?", "鎖定設備"},
    // ethereum.c ethereum.c
    {"message", "消息"},
    // layout2.c
    {"minute", "分鐘"},
    //
    {"minutes", "分鐘"},
    //
    {"mnemonic", "助記詞"},
    // cosmos.c
    {"none", "無"},
    //
    {"number of recovery phrase", "助記詞位數"},
    // recovery.c
    {"of your recovery phrase", "助記詞"},
    // recovery.c
    {"on your computer", "在電腦上"},
    // fsm_msg_coin.h
    {"own risk!", "風險"},
    // protect.c
    {"passphrase using", "密語"},
    // ethereum.c
    {"paying up to", "支付最多"},
    // fsm_msg_common.h fsm_msg_common.h
    {"protection?", "保護"},
    // layout2.c
    {"raw units of", ""},
    // fsm_msg_common.h
    {"reboot start", ""},
    // recovery.c
    {"recover the device?", "恢復錢包"},
    // lisk.c
    {"register a delegate?", ""},
    // fsm_msg_common.h
    {"remove current PIN?", ""},
    // fsm_msg_common.h
    {"restore data err", ""},
    //
    {"restore wallet", "恢復已擁有的錢包"},
    // layout2.c
    {"s", "秒"},
    // layout2.c protect.c
    {"second", "秒"},
    // fsm_msg_common.h
    {"seed import failed", ""},
    // fsm_msg_common.h
    {"seed import success", ""},
    // signing.c
    {"segwit input amount", ""},
    // fsm_msg_common.h
    {"send entropy?", "發送熵值"},
    {"sender", "發送方"},
    // fsm_msg_common.h
    {"set a new wipe code?", ""},
    // fsm_msg_common.h
    {"set new PIN?", "設置PIN碼"},
    // fsm_msg_common.h
    {"status always?", ""},
    // bl_check.c
    {"successfully", "成功"},
    // fsm_msg_common.h fsm_msg_common.h
    {"the U2F counter?", ""},
    // protect.c
    {"the computer's", "使用電腦的"},
    // recovery.c recovery.c
    {"the one in the device.", ""},
    // reset.c
    {"the only way to recover", "助記詞是找回資產唯一"},
    //
    {"the passphrase!", "密語"},
    // layout2.c
    {"the same mosaic", ""},
    //
    {"times", "次"},
    //
    {"timestamp:", "時間戳:"},
    //
    {"to back", "鍵返回"},
    //
    {"to confirm", "鍵確認"},
    // protect.c
    {"to continue ...", " "},
    //
    {"to creat wallet", "開始創建新錢包"},
    // ethereum.c
    {"to new contract?", "新合約"},
    //
    {"to scroll down", "鍵查看下一頁"},
    //
    {"to scroll up", "鍵查看上一頁"},
    //
    {"to try", " "},
    // lisk.c
    {"to:", ""},
    // ethereum.c
    {"token", "代幣"},
    //
    {"transaction is set to", "被設定為"},
    // layout2.c layout2.c
    {"user: ", "用戶:"},
    // u2f.c
    {"was used to register", "註冊"},
    // fsm_msg_common.h
    {"wipe code?", "擦除PIN碼"},
    // fsm_msg_common.h
    {"wipe the device?", "擦除設備"},
    //
    {"word", "助記詞"},
    //
    {"words", "助記詞"},
    //
    {"you still have ", "還有"},
    // reset.c
    {"your asset,Keep it safe", "憑證,請妥善保管"},
};

int LANGUAGE_ITEMS = sizeof(languages) / sizeof(languages[0]);