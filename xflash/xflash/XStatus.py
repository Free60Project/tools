STATUS_ILL_LOG  = 0x800
STATUS_PIN_WP_N = 0x400
STATUS_PIN_BY_N = 0x200
STATUS_INT_CP   = 0x100
STATUS_ADDR_ER  = 0x080
STATUS_BB_ER    = 0x040
STATUS_RNP_ER   = 0x020
STATUS_ECC_ER   = 0x01c
STATUS_WR_ER    = 0x002
STATUS_BUSY     = 0x001

STATUS_OK       = (STATUS_PIN_BY_N)
STATUS_ERROR    = (STATUS_ILL_LOG |
                  STATUS_ADDR_ER |
                  STATUS_BB_ER |
                  STATUS_RNP_ER |
                  STATUS_ECC_ER |
                  STATUS_WR_ER)

def statusIsError(status):
    if status & STATUS_ERROR != 0:
        return True
    if status & STATUS_OK == 0:
        return True
    return False

def statusHasBit(status, bit):
    return status & bit != 0