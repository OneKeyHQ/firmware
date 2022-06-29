import json
import requests
from sqlalchemy import create_engine
from sqlalchemy.orm import declarative_base, sessionmaker
from sqlalchemy import Column, Integer, Text, LargeBinary
from trezorlib._internal import firmware_headers
from trezorlib import firmware, cosi
from trezorlib.cli.special import _ed25519_global_combine


Base = declarative_base()
engine = create_engine('sqlite:///cosign.db', echo=True)


class HandledEvent(Base):
    __tablename__ = 'handled_event'
    id = Column(Integer, primary_key=True, autoincrement=True)
    event_ts = Column(Text, unique=True, default=None)


class DownloadedFile(Base):
    __tablename__ = 'downloaded_file'
    id = Column(Integer, primary_key=True, autoincrement=True)
    url = Column(Text, default=None, unique=True)
    data = Column(LargeBinary, default=None)


def fetch_file(client, url):
    with Session() as session:
        obj = session.query(DownloadedFile).filter_by(url=url).first()
        if obj is not None:
            return obj
    resp = requests.get(url, headers={'Authorization': 'Bearer %s' % client.token}, timeout=10)
    with Session() as session:
        obj = DownloadedFile(url=url, data=resp.content)
        session.add(obj)
        session.commit()
        return obj


def fetch_files_from_msgs(client, msgs, retry_times=3):
    for msg in msgs:
        files = msg.get("files")
        if files is None:
            continue
        if len(files) != 1:
            raise Exception("每条消息仅能包含一个文件")
        url = files[0]['url_private_download']
        for _ in range(retry_times):
            try:
                fetch_file(client, url)
                break
            except:
                continue


class Cosign(Base):
    __tablename__ = 'cosign'
    id = Column(Integer, primary_key=True, autoincrement=True)
    ts = Column(Text, unique=True, default=None)
    ufw_data = Column(LargeBinary, default=None)
    ufw_url = Column(Text, None)
    cm1_data = Column(Text, default=None)
    cm1_url = Column(Text, None)
    cm2_data = Column(Text, default=None)
    cm2_url = Column(Text, None)
    cm3_data = Column(Text, default=None)
    cm3_url = Column(Text, None)
    cm4_data = Column(Text, default=None)
    cm4_url = Column(Text, None)
    gcm_data = Column(LargeBinary, default=None)
    sig1_data = Column(Text, default=None)
    sig1_url = Column(Text, default=None)
    sig2_data = Column(Text, default=None)
    sig2_url = Column(Text, default=None)
    sig3_data = Column(Text, default=None)
    sig3_url = Column(Text, default=None)
    sig4_data = Column(Text, default=None)
    sig4_url = Column(Text, default=None)
    csig = Column(Text, default=None)
    sfw_data = Column(LargeBinary, default=None)
    sfw_url = Column(Text, None)
    last_handled_msg_ts = Column(Text, default=None)

    @property
    def pk1(self):
        if self.cm1_data is None:
            return None
        return json.loads(self.cm1_data)['pubkey']

    @property
    def pk2(self):
        if self.cm2_data is None:
            return None
        return json.loads(self.cm2_data)['pubkey']

    @property
    def pk3(self):
        if self.cm3_data is None:
            return None
        return json.loads(self.cm3_data)['pubkey']

    @property
    def pk4(self):
        if self.cm3_data is None:
            return None
        return json.loads(self.cm4_data)['pubkey']

    @property
    def pks(self):
        return [pk for pk in [self.pk1, self.pk2, self.pk3, self.pk4] if pk is not None]

    @property
    def all_pks(self):
        assert(len(self.pks) == 4)
        return self.pks

    @property
    def pks_in_bytes(self):
        return [bytes.fromhex(pk) for pk in self.pks]

    @property
    def all_pks_in_bytes(self):
        assert(len(self.pks_in_bytes) == 4)
        return self.pks_in_bytes

    @property
    def cm1(self):
        if self.cm1_data is None:
            return None
        return json.loads(self.cm1_data)['R']

    @property
    def cm2(self):
        if self.cm2_data is None:
            return None
        return json.loads(self.cm2_data)['R']

    @property
    def cm3(self):
        if self.cm3_data is None:
            return None
        return json.loads(self.cm3_data)['R']

    @property
    def cm4(self):
        if self.cm4_data is None:
            return None
        return json.loads(self.cm4_data)['R']

    @property
    def cms(self):
        return [c for c in [self.cm1, self.cm2, self.cm3, self.cm4] if c is not None]

    @property
    def cms_in_bytes(self):
        return [bytes.fromhex(cm) for cm in self.all_cms]

    @property
    def sig1_pk(self):
        if self.sig1_data is None:
            return None
        return json.loads(self.sig1_data)['signing_pubkey']

    @property
    def sig2_pk(self):
        if self.sig2_data is None:
            return None
        return json.loads(self.sig2_data)['signing_pubkey']

    @property
    def sig3_pk(self):
        if self.sig3_data is None:
            return None
        return json.loads(self.sig3_data)['signing_pubkey']

    @property
    def sig4_pk(self):
        if self.sig4_data is None:
            return None
        return json.loads(self.sig4_data)['signing_pubkey']

    @property
    def sig1(self):
        if self.sig1_data is None:
            return None
        return json.loads(self.sig1_data)['single_signature']

    @property
    def sig2(self):
        if self.sig2_data is None:
            return None
        return json.loads(self.sig2_data)['single_signature']

    @property
    def sig3(self):
        if self.sig3_data is None:
            return None
        return json.loads(self.sig3_data)['single_signature']

    @property
    def sig4(self):
        if self.sig4_data is None:
            return None
        return json.loads(self.sig4_data)['single_signature']

    @property
    def sigs(self):
        return [s for s in [self.sig1, self.sig2, self.sig3, self.sig4] if s is not None]

    @property
    def all_sigs(self):
        assert(len(self.sigs) == 4)
        return self.sigs

    @property
    def sigs_in_bytes(self):
        return [bytes.fromhex(s) for s in self.sigs]

    @property
    def all_sigs_in_bytes(self):
        assert(len(self.sigs_in_bytes) == 4)
        return self.sigs_in_bytes

    def cm_done(self):
        return self.cm4_data is not None

    def cm_to_fill(self):
        if self.cm1_data is None:
            return 'cm1_data', 'cm1_url'
        if self.cm2_data is None:
            return 'cm2_data', 'cm2_url'
        if self.cm3_data is None:
            return 'cm3_data', 'cm3_url'
        if self.cm4_data is None:
            return 'cm4_data', 'cm4_url'
        raise Exception("所有Commit都已填充")

    def perform_global_combine(self):
        with open('cm1.json', 'wb') as f:
            f.write(self.cm1_data)
        with open('cm2.json', 'wb') as f:
            f.write(self.cm2_data)
        with open('cm3.json', 'wb') as f:
            f.write(self.cm3_data)
        with open('cm4.json', 'wb') as f:
            f.write(self.cm4_data)
        cms = ['cm1.json', 'cm2.json', 'cm3.json', 'cm4.json']
        _, d = _ed25519_global_combine(cms)
        return bytes(d, encoding='utf8')

    def sig_done(self):
        return self.sig4_data is not None

    def sig_to_fill(self):
        if self.sig1_data is None:
            return 'sig1_data', 'sig1_url'
        if self.sig2_data is None:
            return 'sig2_data', 'sig2_url'
        if self.sig3_data is None:
            return 'sig3_data', 'sig3_url'
        if self.sig4_data is None:
            return 'sig4_data', 'sig4_url'
        raise Exception("所有Signature都已填充")

    def ufw_message(self):
        msg = '待签镜像已保存, Digest:\n'
        fw = firmware_headers.parse_image(self.ufw_data)
        msg += fw.digest().hex()
        return msg

    def commit_message(self):
        msg = 'Commit已保存\n'
        if self.cm1_data is None:
            msg += 'commit1提供者: 无\n'
        else:
            msg += 'commit1提供者: %s\n' % self.pk1

        if self.cm2_data is None:
            msg += 'commit2提供者: 无\n'
        else:
            msg += 'commit2提供者: %s\n' % self.pk2

        if self.cm3_data is None:
            msg += 'commit3提供者: 无\n'
        else:
            msg += 'commit3提供者: %s\n' % self.pk3

        if self.cm4_data is None:
            msg += 'commit4提供者: 无\n'
        else:
            msg += 'commit4提供者: %s\n' % self.pk4
        return msg

    def messages(self):
        if self.cm1_data is None:
            return [('msg', self.ufw_message()),]
        if self.cm2_data is None:
            return [('msg', self.commit_message()),]
        if self.cm3_data is None:
            return [('msg', self.commit_message()),]
        if self.cm4_data is None:
            return [('msg', self.commit_message()),]
        if self.sig1_data is None:
            msgs = [('msg', self.commit_message()),]
            brief_digest = json.loads(self.gcm_data)['digest'][:4]
            brief_gpk = json.loads(self.gcm_data)['global_pubkey'][:4]
            brief_gcm = json.loads(self.gcm_data)['global_commitment'][:4]
            fname = 'global_%s_%s_%s.json' % (brief_digest, brief_gpk, brief_gcm)
            msgs.append(('file', {'name': fname, 'content': self.gcm_data}))
            return msgs
        if self.sig2_data is None:
            return [('msg', '签名已保存'),]
        if self.sig3_data is None:
            return [('msg', '签名已保存'),]
        if self.sig4_data is None:
            return [('msg', '签名已保存'),]
        if self.sfw_data is not None:
            return [('file', {'name': 'signed.bin', 'content': self.sfw_data}),]
        return 'msg', 'error'


Base.metadata.create_all(engine)
Session = sessionmaker(bind=engine)


def try_record_event(event_ts):
    with Session() as session:
        r = session.query(HandledEvent).filter_by(event_ts=event_ts).first()
        if r is None:
            r = HandledEvent(event_ts=event_ts)
            session.add(r)
            session.commit()
            return True
        else:
            return False


def update_users_name(client):
    pass


def create_cosign(client, msgs):
    ts = msgs[0].get("ts")
    files = msgs[0].get("files")
    if files is None:
        raise Exception("未找到文件")
    if len(files) > 1:
        raise Exception("只能处理一个文件")
    obj = fetch_file(client, files[0]['url_private_download'])
    firmware_headers.parse_image(obj.data)
    with Session() as session:
        r = session.query(Cosign).filter_by(ts=ts).first()
        if r is None:
            r = Cosign(ts=ts, ufw_data=obj.data, ufw_url=obj.url,
                last_handled_msg_ts=ts)
            session.add(r)
            session.commit()


def strip_msgs(msgs, ts):
    if ts is None:
        return msgs
    for i in range(len(msgs)):
        if msgs[i]['ts'] == ts:
            return msgs[i+1:]
    raise Exception("没有匹配的消息")


def parse_commit_msg(client, msg):
    files = msg.get("files")
    if files is None:
        return False, None, None
    if len(files) > 1:
        return False, None, None
    obj = fetch_file(client, files[0]['url_private_download'])
    try:
        json.loads(obj.data)
    except:
        return False, None, None
    if json.loads(obj.data).get('R') is None:
        return False, None, None
    return True, obj.data, obj.url


def update_commit(client, msg, query):
    obj = query.first()
    is_cm, cm_data, cm_url = parse_commit_msg(client, msg)
    if not is_cm:
        return False
    if is_cm and not obj.cm_done():
        field_cm_data, field_cm_url = obj.cm_to_fill()
        query.update({field_cm_data: cm_data, field_cm_url: cm_url,
            'last_handled_msg_ts': msg['ts']})
    if obj.cm_done() and obj.gcm_data is None:
        gcm_data = obj.perform_global_combine()
        query.update({'gcm_data': gcm_data})
    return True


def parse_sig_msg(client, msg):
    files = msg.get("files")
    if files is None:
        return False, None, None
    if len(files) > 1:
        return False, None, None
    obj = fetch_file(client, files[0]['url_private_download'])
    try: 
        json.loads(obj.data)
    except (json.decoder.JSONDecodeError, UnicodeDecodeError):
        return False, None, None
    if json.loads(obj.data).get('single_signature') is None:
        return False, None, None
    return True, obj.data, obj.url


def update_sig(client, msg, query):
    obj = query.first()
    is_sig, sig_data, sig_url = parse_sig_msg(client, msg)
    if not is_sig:
        return False
    if not obj.sig_done():
        field_sig_data, field_sig_url = obj.sig_to_fill()
        query.update({field_sig_data: sig_data, field_sig_url: sig_url,
            'last_handled_msg_ts': msg['ts']})
    if obj.sig_done() and obj.csig is None:
        gcm = bytes.fromhex(json.loads(obj.gcm_data)['global_commitment'])
        csig = cosi.combine_sig(gcm, obj.all_sigs_in_bytes)
        sigmask = 0
        for pk in obj.all_pks_in_bytes:
            idx = firmware.V2_BOOTLOADER_KEYS.index(pk)
            sigmask |= 1 << idx
        fw = firmware_headers.parse_image(obj.ufw_data)
        fw.rehash()
        fw.insert_signature(csig, sigmask)
        cosi.verify(fw.header.signature, fw.digest(), 4,
            firmware.V2_BOOTLOADER_KEYS, sigmask)
        sfw_data = fw.dump()
        query.update({'csig': csig.hex(), 'sfw_data': sfw_data})
    return True


def process_cosign(client, msgs):
    thread_ts = msgs[0].get("ts")
    with Session() as session:
        q = session.query(Cosign).filter_by(ts=thread_ts)
        msgs = strip_msgs(msgs, q.first().last_handled_msg_ts)
        for msg in msgs:
            if update_commit(client, msg, q):
                continue
            if update_sig(client, msg, q):
                continue
        session.commit()


def reply_cosign(client, channel, thread_ts):
    with Session() as session:
        r = session.query(Cosign).filter_by(ts=thread_ts).first()
        for mtype, msg in r.messages():
            if mtype == 'msg':
                client.chat_postMessage(channel=channel, thread_ts=thread_ts,
                    text=msg)
            if mtype == 'file':
                client.files_upload(channels=channel, thread_ts=thread_ts,
                    file=msg['content'], filename=msg['name'])


if __name__ == '__main__':
    with Session() as session:
        r = session.query(Cosign).filter_by(id=4).first()
        print(r.pks)
        print(r.cms)
        print(r.all_sigs)
