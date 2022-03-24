def _init():
    global _global_dict
    global _global_list
    _global_list = []
    _global_dict = {}

def set_value(name, value):
    _global_dict[name] = value
    _global_list.append(value)

def get_value(name, defValue=None):
    try:
        return _global_dict[name]
    except KeyError:
        return defValue

def get_dictionary():
    return _global_dict

def get_list():
    return _global_list

def del_all():
    _global_dict.clear()
    _global_list.clear()
