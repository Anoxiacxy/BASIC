import pickle
import base64
import sys
import subprocess
import time

YOUR_BASEPATH = 'ut'
VERBOSE = False

def info(s):
    print('[Info] {}'.format(s))

def fatal(s):
    print('[FATAL ERROR] {}'.format(s))
    sys.exit(0)

def judge(testcase):
    build_timeout = 5
    testname, inputctx, retcode, inputcode = testcase
    inputcode = open(YOUR_BASEPATH + '/' + "testcases" + "/" + testname).read()
    if (testname == "control_1.txt"):
        print('[FAIL] {}, std_ret: {}, your_ret: {}'.format(testname, retcode, 0))
        return False
    if VERBOSE:
        print('[Verbose] =======Build Stage START =======')
        process = subprocess.run(['./build'], input=inputcode.encode(), cwd=YOUR_BASEPATH, stderr=subprocess.DEVNULL,timeout=build_timeout)
    else: 
        process = subprocess.run(['./build'], input=inputcode.encode(), cwd=YOUR_BASEPATH, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, timeout=build_timeout)
    print('[Verbose] =======Build Stage FINISH=======\n') if VERBOSE else None
    
    if process.returncode != 0 and retcode == -1:
        print('[PASS] {}'.format(testname))
        return True
    elif retcode == -1:
        print('[FAIL] {}, std: fail to compile'.format(testname))
        return False

    if process.returncode != 0:
        fatal('Unable to build with build stage exitcode: {}'.format(process.returncode))
    if VERBOSE:
        print('[Verbose] =======Simulation Stage START =======')
        process = subprocess.run(['./simulate'], input=(inputctx + '\n').encode(), cwd=YOUR_BASEPATH, stderr=subprocess.DEVNULL, timeout=build_timeout)
    else:
        process = subprocess.run(['./simulate'], input=(inputctx + '\n').encode(), cwd=YOUR_BASEPATH, stderr=subprocess.DEVNULL, timeout=build_timeout)

    print('[Verbose] =======Simulation Stage FINISH=======\n') if VERBOSE else None

    user_ret = process.returncode
    if user_ret == retcode:
        print('[PASS] {}'.format(testname))
        return True
    else:
        print('[FAIL] {}, std_ret: {}, your_ret: {}'.format(testname, retcode, user_ret))
        return False
    

testdata = "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MTsgaSA8IG4KOCBMRVQgc3VtID0gc3VtICsgaQo5IEVORCBGT1IKMTAgRVhJVCBzdW0gCpR0lCiMC2Jhc2ljXzkudHh0lGgaSwaMujEgUkVNIGJhc2ljXzkudHh0CjIgUkVNIGlucHV0OiA1CjMgUkVNIGFzZmVza2VqZmV3ZjQgSU5QVVQgbgo1IExFVCBpID0gMQo2IExFVCBzdW0gPSAwCjcgRk9SIGkgPSBpICsgMTsgaSA8IG4KOCBJRiBpIC0gMiAqIChpIC8gMikgIT0gMCBUSEVOIDcKOSBMRVQgc3VtID0gc3VtICsgaQoxMCBFTkQgRk9SCjExIEVYSVQgc3VtCpR0lCiMDGJhc2ljXzEwLnR4dJRoGksejGAxIFJFTSBiYXNpY18xMC50eHQKMiBSRU0gaW5wdXQ6IDUKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCBuCjUgTEVUIHN1bSA9IG4gKiBuICsgbgo2IEVYSVQgc3VtIAqUdJQojA1jb250cm9sXzEudHh0lIwCMTOUTUDsWO0DAAAxIFJFTSBjb250cm9sXzEudHh0CjIgUkVNIGlucHV0OiAxMwozIFJFTSBhc2Zlc2tlamZld2Y0IElOUFVUIE4KNSBMRVQgaCA9IDkKNiBMRVQgaSA9IDEwCjcgTEVUIGogPSAxMQo4IExFVCBrID0gMTIKOSBMRVQgdG90YWwgPSAwCjEwIExFVCBhID0gMQoxMSBMRVQgYiA9IDEKMTIgTEVUIGMgPSAxCjEzIExFVCBkID0gMQoxNCBMRVQgZSA9IDEKMTUgTEVUIGYgPSAxCjE2IEZPUiBhID0gYSArIDE7IGEgPD0gTgoxNyAgTEVUIGIgPSAxCjE4ICBGT1IgYiA9IGIgKyAxOyBiIDw9IE4KMTkgIExFVCBjID0gMQoyMCAgICBGT1IgYyA9IGMgKyAxOyBjIDw9IE4KMjEgICAgICBMRVQgZCA9IDEKMjIgICAgICBGT1IgZCA9IGQgKyAxOyBkIDw9IE4KMjMgICAgICAgIExFVCBlID0gMQoyNCAgICAgICAgRk9SIGUgPSBlICsgMTsgZSA8PSBOCjI1ICAgICAgICAgIExFVCBmID0gMQoyNiAgICAgICAgICBGT1IgZiA9IGYgKyAxOyBmIDw9IE4KMjcgICAgICAgICAgICBJRiAoYT09YiB8fCBhPT1jIHx8ICBhPT1kIHx8ICBhPT1lIHx8ICBhPT1mIHx8ICBhPT1oIHx8ICBhPT1pIHx8ICBhPT1qIHx8ICBhPT1rIHx8ICBiPT1jIHx8ICBiPT1kIHx8ICBiPT1lIHx8ICBiPT1mIHx8ICBiPT1oIHx8ICBiPT1pIHx8ICBiPT1qIHx8ICBiPT1rIHx8ICBjPT1kIHx8ICBjPT1lIHx8ICBjPT1mIHx8ICBjPT1oIHx8ICBjPT1pIHx8ICBjPT1qIHx8ICBjPT1rIHx8ICBkPT1lIHx8ICBkPT1mIHx8ICBkPT1oIHx8ICBkPT1pIHx8ICBkPT1qIHx8ICBkPT1rIHx8ICBlPT1mIHx8ICBlPT1oIHx8ICBlPT1pIHx8ICBlPT1qIHx8ICBlPT1rIHx8ICBmPT1oIHx8ICBmPT1pIHx8ICBmPT1qIHx8ICBmPT1rIHx8ICBpPT1qIHx8ICBoPT1rKSBUSEVOIDI5CjI4ICAgICAgICAgICAgTEVUIHRvdGFsID0gdG90YWwgKyAxCjI5ICAgICAgICAgIEVORCBGT1IKMzAgICAgICAgIEVORCBGT1IKMzEgICAgICBFTkQgRk9SIAozMiAgICBFTkQgRk9SCjMzICBFTkQgRk9SCjM0IEVORCBGT1IKMzUgRVhJVCB0b3RhbAqUdJQojA1jb250cm9sXzIudHh0lIwBNpRLBoxzMSBSRU0gY29udHJvbF8yLnR4dAoyIFJFTSBpbnB1dDogNgozIFJFTSBhc2Zlc2tlamZld2Y1IElOUFVUIG4KNyBJRiBuID4gNSBUSEVOIDExCjggTEVUIG4gPSA1CjEwIEVYSVQgbgoxMSBHT1RPIDEwCpR0lCiMDWNvbnRyb2xfMy50eHSUjAQxMCA1lEsAjOQxIFJFTSBjb250cm9sXzMudHh0CjIgUkVNIGlucHV0OiAxMCA1CjMgUkVNIGFzZmVza2VqZmV3ZjQgSU5QVVQgbSxuCjUgRk9SIG0gPSBtIC0gKG0gLyBuKSAqIG47IG4gPiAwCjYgSUYgbSA+PSBuIFRIRU4gMTAKNyBMRVQgdG1wID0gbQo4IExFVCBtID0gbgo5IExFVCBuID0gdG1wCjEwIElGIG4gPT0gMCBUSEVOIDE0CjExIEVORCBGT1IKMTIgRVhJVCBtCjE0IExFVCBuID0gLTEKMTUgR09UTyAxMQqUdJQojA1jb250cm9sXzQudHh0lGgaSxBYBQEAADEgUkVNIGNvbnRyb2xfNC50eHQKMiBSRU0gaW5wdXQ6IDUKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCBuCjUgR09UTyA5CjYgR09UTyAxMgo3IExFVCBuID0gKG4gKiA4KSArIG4gLyA0CjggR09UTyAxMQo5IExFVCBuID0gbiAqIDIgKyAxCjEwIEdPVE8gNgoxMSBHT1RPIDE2CjEyIExFVCBuID0gKG4gLSA2KSAvIDIgCjEzIElGIG4gPT0gMiBUSEVOIDcKMTQgRVhJVCAyNAoxNiBJRiBuIDw9IDEwIFRIRU4gMjEKMjAgRVhJVCBuCjIxIEVYSVQgbiAtIDEwCpR0lCiMDWNvbnRyb2xfNS50eHSUaBpLClhbAQAAMSBSRU0gY29udHJvbF81LnR4dAoyIFJFTSBpbnB1dDogNQozIFJFTSBhc2Zlc2tlamZld2Y0IElOUFVUIG4KNSBMRVQgYSA9IDEKNiBMRVQgYiA9IDEKNyBMRVQgYyA9IDEKOCBMRVQgZCA9IDEKOSBMRVQgc3VtID0gMAoxMCBGT1IgYSA9IGEgKyAxOyBhIDw9IG4KMTEgTEVUIGIgPSBhCjEyIEZPUiBiID0gYiArIDE7IGIgPD0gbgoxMyBMRVQgYyA9IGIKMTQgRk9SIGMgPSBjICsgMTsgYyA8PSBuCjE1ICBJRiBhICE9IGIgJiYgYiAhPSBjICYmIGEgIT0gYyBUSEVOIDE3CjE2ICBHT1RPIDE4CjE3ICBMRVQgc3VtID0gc3VtICsgMQoxOCBFTkQgRk9SCjE5IEVORCBGT1IKMjAgRU5EIEZPUgoyMSBFWElUIHN1bQqUdJQojAhvcF8xLnR4dJSMBDUgIDeUS0OMYjEgUkVNIG9wXzEudHh0CjIgUkVNIGlucHV0OiA1ICA3CjMgUkVNIGFzZmVza2VqZmV3ZjQgSU5QVVQgYSxiCjUgTEVUIGMgPSAoYSArIGIpICogYSArIGIKNiBFWElUIGMKlHSUKIwIb3BfMi50eHSUjAMxIDGUSwNY6QUAADEgUkVNIG9wXzIudHh0CjIgUkVNIGlucHV0OiAxIDEKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCBBCjUgSU5QVVQgQgo2IExFVCBDID0gMQo3IExFVCBBID0gKCgoKCgoKChDIC0gQSArIEIpIC0gKEEgKyBCKSkgKyAoKEMgLSBBICsgQikgLSAoQSArIEIpKSkgKyAoKChDIC0gQSArIEIpIC0gKEEgKyBCKSkgKyAoQyAtIEEgKyBCKSkpIC0gKCgoKEEgKyBCKSArIChDIC0gQSArIEIpKSAtIChBICsgQikpICsgKCgoQyAtIEEgKyBCKSAtIChBICsgQikpICsgKEMgLSBBICsgQikpKSkgLSAoKCgoKEEgKyBCKSArIChDIC0gQSArIEIpKSAtICgoQSArIEIpICsgKEMgLSBBICsgQikpKSAtICgoKEEgKyBCKSArIChDIC0gQSArIEIpKSAtIChBICsgQikpKSArICgoKChDIC0gQSArIEIpIC0gKEEgKyBCKSkgKyAoQyAtIEEgKyBCKSkgLSAoKChBICsgQikgKyAoQyAtIEEgKyBCKSkgLSAoQSArIEIpKSkpKSArICgoKCgoKEMgLSBBICsgQikgLSAoQSArIEIpKSArICgoQyAtIEEgKyBCKSAtIChBICsgQikpKSArICgoKEMgLSBBICsgQikgLSAoQSArIEIpKSArIChDIC0gQSArIEIpKSkgLSAoKCgoQSArIEIpICsgKEMgLSBBICsgQikpIC0gKEEgKyBCKSkgKyAoKChDIC0gQSArIEIpIC0gKEEgKyBCKSkgKyAoQyAtIEEgKyBCKSkpKSAtICgoKCgoQSArIEIpICsgKEMgLSBBICsgQikpIC0gKEEgKyBCKSkgKyAoKChDIC0gQSArIEIpIC0gKEEgKyBCKSkgKyAoQyAtIEEgKyBCKSkpIC0gKCgoKEEgKyBCKSArIChDIC0gQSArIEIpKSAtIChBICsgQikpICsgKCgoQyAtIEEgKyBCKSAtIChBICsgQikpICsgKEMgLSBBICsgQikpKSkpKSAtICgoKCgoKChBICsgQikgKyAoQyAtIEEgKyBCKSkgLSAoKEEgKyBCKSArIChDIC0gQSArIEIpKSkgLSAoKChBICsgQikgKyAoQyAtIEEgKyBCKSkgLSAoQSArIEIpKSkgKyAoKCgoQyAtIEEgKyBCKSAtIChBICsgQikpICsgKEMgLSBBICsgQikpIC0gKCgoQSArIEIpICsgKEMgLSBBICsgQikpIC0gKEEgKyBCKSkpKSArICgoKCgoQyAtIEEgKyBCKSAtIChBICsgQikpICsgKEMgLSBBICsgQikpIC0gKCgoQSArIEIpICsgKEMgLSBBICsgQikpIC0gKEEgKyBCKSkpICsgKCgoKEMgLSBBICsgQikgLSAoQSArIEIpKSArIChDIC0gQSArIEIpKSAtICgoKEEgKyBCKSArIChDIC0gQSArIEIpKSAtIChBICsgQikpKSkpICsgKCgoKCgoQyAtIEEgKyBCKSAtIChBICsgQikpICsgKChDIC0gQSArIEIpIC0gKEEgKyBCKSkpICsgKCgoQyAtIEEgKyBCKSAtIChBICsgQikpICsgKEMgLSBBICsgQikpKSAtICgoKChBICsgQikgKyAoQyAtIEEgKyBCKSkgLSAoQSArIEIpKSArICgoKEMgLSBBICsgQikgLSAoQSArIEIpKSArIChDIC0gQSArIEIpKSkpIC0gKCgoKChBICsgQikgKyAoQyAtIEEgKyBCKSkgLSAoQSArIEIpKSArICgoKEMgLSBBICsgQikgLSAoQSArIEIpKSArIChDIC0gQSArIEIpKSkgLSAoKCgoQSArIEIpICsgKEMgLSBBICsgQikpIC0gKEEgKyBCKSkgKyAoKChDIC0gQSArIEIpIC0gKEEgKyBCKSkgKyAoQyAtIEEgKyBCKSkpKSkpKQo4IEVYSVQgQQqUdJQojAhvcF8zLnR4dJSMAzQgNpRLNYyWMSBSRU0gb3BfMy50eHQKMiBSRU0gaW5wdXQ6IDQgNgozIFJFTSBhc2Zlc2tlamZld2Y0IElOUFVUIHgsIHkKNSBJRiAoKHggPCB5KSAmJiAoeCArIDUgPj0geSkpIFRIRU4gNwo2IExFVCB4ID0geCArIDIKNyBMRVQgeSA9IHkgKyAxCjggRVhJVCB4ICsgeSAqIHkKlHSUKIwIb3BfNC50eHSUjAM0IDaUTZgHjIcxIFJFTSBvcF80LnR4dAoyIFJFTSBpbnB1dDogNCA2CjMgUkVNIGFzZmVza2VqZmV3ZjQgSU5QVVQgeCwgeQo1IEVYSVQgKCgoeCArICB5KSAgKiB5KSAtIHggKiB5KSAqICgoeCAtIHkpICsgKHkgIC0geCkgKiAoeCArICh5ICogeCkpKQqUdJQojAhvcF81LnR4dJSMAzQgNpRLE4zRMSBSRU0gb3BfNS50eHQKMiBSRU0gaW5wdXQ6IDQgNgozIFJFTSBhc2Zlc2tlamZld2Y0IElOUFVUIG4sIG0KNSBMRVQgaSA9IDAKNiBMRVQgc3VtID0gMAo3IEZPUiBpID0gaSArIDE7IChpIDw9IDUpICYmIChpIDwgbiAqIChuICogbSAtIG0pIC0gbiAqIG0gLSBuKQo4IExFVCBzdW0gPSBzdW0gKyBpICogaQo5IEVORCBGT1IKMTAgRVhJVCBzdW0gLSAoaSAqIDYpCgqUdJQojAhvcF82LnR4dJSMAzggNpRLAIySMSBSRU0gb3BfNi50eHQKMiBSRU0gaW5wdXQ6IDggNgozIFJFTSBhc2Zlc2tlamZld2Y0IElOUFVUIGEsIGIKNSBJRiBhID49IGIgJiYgYSAtIDEgPT0gYiBUSEVOIDgKNiBMRVQgYyA9IGIgLyBhCjcgR09UTyA5CjggTEVUIGMgPSBhICogYgo5IEVYSVQgYwqUdJQojAhvcF83LnR4dJSMAjEwlEsEjIIxIFJFTSBvcF83LnR4dAoyIFJFTSBpbnB1dDogMTAKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCBuIAo1IExFVCBpID0gMQo2IEZPUiBpID0gaSArIDE7IGkgPCBuCjcgTEVUIG4gPSBuIC0gaQo4IEVORCBGT1IKOSBFWElUIG4KlHSUKIwIb3BfOC50eHSUjAMzIDaUSwGMbTEgUkVNIG9wXzgudHh0CjIgUkVNIGlucHV0OiAzIDYgCjMgUkVNIGFzZmVza2VqZmV3ZjQgSU5QVVQgbSwgbgo1IExFVCBleGl0ID0gbS8yICsgKG4vMiAtIG0pICogNQo2IEVYSVQgZXhpdAqUdJQojAhvcF85LnR4dJSMBDggMTCUSwSMuDEgUkVNIG9wXzkudHh0CjIgUkVNIGlucHV0OiA4IDEwCjMgUkVNIGFzZmVza2VqZmV3ZjQgSU5QVVQgbSxuCjUgSUYgbSA+PSBuIFRIRU4gOAo2IElGIG0gPCBuIC0gMiB8fCBtID09IG4gLSAyIFRIRU4gMTAKNyBHT1RPIDkKOCBMRVQgbSA9IG0gLSBuCjkgRVhJVCBtCjEwIExFVCBtID0gbiArIDIgLSBtCjExIEdPVE8gOQqUdJQojAlvcF8xMC50eHSUjAc3IDggMiAzlEsMjKIxIFJFTSBvcF8xMC50eHQKMiBSRU0gaW5wdXQ6IDcgOCAyIDMKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCBtLCBuLCBwLCBxCjUgTEVUIHggPSAobSArIG4pL3AgKyAobSArIG4pLyBxCjYgTEVUIHkgPSB4ICogMiAtIChtICsgbikgKiAocCArIHEpIC8gKHAgKiBxKQo3IEVYSVQgeQqUdJQojApzZW1hXzEudHh0lGgWSv////+MezEgUkVNIHNlbWFfMS50eHQKMiBSRU0gaW5wdXQ6IAozIFJFTSBhc2Zlc2tlamZld2Y0IExFVCBuID0gMCwgbSA9IDEKNSBMRVQgcmV0ID0gTiAqIG0KNiBFWElUIHJldCAKNyBSRU0gdW5kZWZpbmVkIHZhcmlhYmxlCpR0lCiMCnNlbWFfMi50eHSUjAM0IDCUSwSMlzEgUkVNIHNlbWFfMi50eHQKMiBSRU0gaW5wdXQ6IDQgMAozIFJFTSBhc2Zlc2tlamZld2Y0IElOUFVUIG4sIG0KNSBJRiBuID49IDAgfHwgbiAvIG0gIT0gMSBUSEVOIDcKNiBFWElUIDYKNyBFWElUIG4gKyBtCjggUkVNIFNob3J0IGNpcmN1aXQgZXZhbHVhdGlvbgqUdJQojApzZW1hXzMudHh0lGgWSwCMtTEgUkVNIHNlbWFfMy50eHQKMiBSRU0gaW5wdXQ6CjMgUkVNIGFzZmVza2VqZmV3ZjQgSUYgMiA+IDEgVEhFTiAxMAo1IFJFTSBpdCBpcyBhIHN0cmFuZ2UgdGVzdGNhc2UKNiBSRU0gIGJ1dCBJIGxpa2UgaXQKOSBSRU0gaGFoYQoxMCBSRU0gIGFmdGVyIHRoZSBiaWdnZXN0IGxpbmUKMTEgUkVNIFRoZW4gRVhJVCAwIQqUdJQojApzZW1hXzQudHh0lIwDNCAwlEr/////jOkxIFJFTSBzZW1hXzQudHh0CjIgUkVNIGlucHV0OiA0IDAKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCB4LCB5CjUgUkVNIEkgd2FudCB0byBzZWUgb3BlcmF0aW9uCjYgTEVUIHRydWUgID0gKCh4ICsgeSkgICogKHgtICB5KSAgKiB5ICAtICAoKCgoeCAgLSB5KSAqICgoKHkgLSB4KSkpKSkpKQo3IExFVCBmYWlsICA9ICAoKCgoKCgoeCAtIHkpICogKCgoeCArICB5KSAvIHgpKSkpKSkpCjggRVhJVCB0cnVlCpR0lCiMCnNlbWFfNS50eHSUaBZK/////4xKMSBSRU0gc2VtYV81LnR4dAoyIFJFTSBpbnB1dDogCjMgUkVNIGFzZmVza2VqZmV3ZjQgTEVUIExFVCA9IDUKNSBFWElUIExFVAqUdJQojApzZW1hXzYudHh0lGgWSv////+MbDEgUkVNIHNlbWFfNi50eHQKMiBSRU0gaW5wdXQ6IAozIFJFTSBhc2Zlc2tlamZld2Y0IExFVCBhcnJheSA9IElOVFs0NV0KNSBMRVQgYXJyYXlbNDVdID0gMzQKNiBFWElUIGFycmF5WzBdCpR0lCiMCnNlbWFfNy50eHSUaBZK/////4xWMSBSRU0gc2VtYV83LnR4dAoyIFJFTSBpbnB1dDogCjMgUkVNIGFzZmVza2VqZmV3ZjQgTEVUIGEgPSA0NQo1IExFVCBiID0gYVswXQo2IEVYSVQgYgqUdJQojApzZW1hXzgudHh0lIwCMTCUSwGM7zEgUkVNIHNlbWFfOC50eHQKMiBSRU0gaW5wdXQ6IDEwCjMgUkVNIGFzZmVza2VqZmV3ZjQgSU5QVVQgbQo1IExFVCBuID0gSU5UW21dCjYgTEVUIG5bMyAqIDNdID0gIDIKNyBMRVQgdG1wID0gSU5UW25bOV1dCjggTEVUIGkgPSAwCjkgRk9SIGkgPSBpICsgMTsgaSA8IG5bOV0KMTAgSUYgaSAlIDIgPT0gMCBUSEVOIDEyCjExIExFVCBuW2ldID0gaQoxMiBFTkQgRk9SCjEzIExFVCBuWzBdID0gOQoxNCBFWElUIG5bMV0KlHSUKIwKc2VtYV85LnR4dJRoFkr/////jEYxIFJFTSBzZW1hXzkudHh0CjIgUkVNIGlucHV0OiAKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCBtLCBuCjUgRVhJVCAKlHSUKIwLc2VtYV8xMC50eHSUaBZK/////4xEMSBSRU0gc2VtYV8xMC50eHQKMiBSRU0gaW5wdXQ6IAozIFJFTSBhc2Zlc2tlamZld2Y0IG4gPSAxMAo1IEVYSVQgbgqUdJQojAtzZW1hXzExLnR4dJSMAi05lEsBjGsxIFJFTSBzZW1hXzExLnR4dAoyIFJFTSBpbnB1dDogLTkKMyBSRU0gYXNmZXNrZWpmZXdmNCBMRVQgTiA9ICgxMCkKNSBJTlBVVCBNCjYgTEVUIE4gID0gKE0pICsgKE4pCjcgRVhJVCBOCpR0lCiMC3NlbWFfMTIudHh0lIwBM5RLBYxcMSBSRU0gc2VtYV8xMi50eHQKMiBSRU0gaW5wdXQ6IDMKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCBhCjUgTEVUIChhKSA9IGEgKyAyCjYgRVhJVCAoKGEpKQqUdJQojAtzZW1hXzEzLnR4dJRoFkr/////jHQxIFJFTSBzZW1hXzEzLnR4dAoyIFJFTSBpbnB1dDogCjMgUkVNIGFzZmVza2VqZmV3ZjQgTEVUIGkgPSAwCjUgRk9SIGkgPSBpICsgMTsgaSA8IDEwCjYgRU5EIEZPUgo3IEVORCBGT1IKOCBFWElUIGkKCpR0lCiMC3NlbWFfMTQudHh0lGgWSv////+MeTEgUkVNIHNlbWFfMTQudHh0CjIgUkVNIGlucHV0OiAKMyBSRU0gYXNmZXNrZWpmZXdmNCBMRVQgaSA9IDAKNSBGT1IgaSA8IDEwOyBpID0gaSArIDEKNiBMRVQgaSA9IGkgKyAyCjcgRU5EIEZPUgo4IEVYSVQgaQqUdJQojAtzZW1hXzE1LnR4dJRoFkr/////jFcxIFJFTSBzZW1hXzE1LnR4dAoyIFJFTSBpbnB1dDogCjMgUkVNIGFzZmVza2VqZmV3ZjQgTEVUIGkgPSAwCjUgTEVUIGogPSBpICsgMQo2IEVYSVQgawqUdJQojAtzZW1hXzE2LnR4dJSMBTEyMzQ2lEr/////jGkxIFJFTSBzZW1hXzE2LnR4dAoyIFJFTSBpbnB1dDogMTIzNDYKMyBSRU0gYXNmZXNrZWpmZXdmNCBFWElUIGkKNSBMRVQgaSA9IDAKNiBMRVQgaiA9IGkgKyAxCjcgUkVNIEVYSVQgagqUdJQojAtzZW1hXzE3LnR4dJSMBzAwMDAwMDCUSwCMazEgUkVNIHNlbWFfMTcudHh0CjIgUkVNIGlucHV0OiAwMDAwMDAwCjMgUkVNIGFzZmVza2VqZmV3ZjQgRVhJVCAwCjUgTEVUIGkgPSAwCjYgTEVUIGogPSBpICsgMQo3IFJFTSBFWElUIGoKlHSUKIwLc2VtYV8xOC50eHSUjAcwMDAwMDAwlEr/////jKExIFJFTSBzZW1hXzE4LnR4dAoyIFJFTSBpbnB1dDogMDAwMDAwMAozIFJFTSBhc2Zlc2tlamZld2Y0IElOUFVUIG4KNSBMRVQgaSA9IDEKNiBMRVQgc3VtID0gMAo3IEZPUiBpID0gaSArIDE7IGkgPCBudHQKOCBMRVQgc3VtID0gc3VtICsgaQo5IEVORCBGT1IKMTAgRVhJVCBzdW0gCpR0lCiMC3NlbWFfMTkudHh0lIwHMDAwMDAwMJRK/////4yEMSBSRU0gc2VtYV8xOS50eHQKMiBSRU0gaW5wdXQ6IDAwMDAwMDAKMyBSRU0gYXNmZXNrZWpmZXdmNCBSRU0gU1RBVFVTCjUgSU5QVVQgbgo3IElGIG4gPiA1IFRIRU4gMTIKOCBMRVQgbiA9IDUKMTAgRVhJVCBuCjExIEdPVE8gMTAKlHSUKIwLc2VtYV8yMC50eHSUjAM0IDaUSv////+MmTEgUkVNIHNlbWFfMjAudHh0CjIgUkVNIGlucHV0OiA0IDYKMyBSRU0gYXNmZXNrZWpmZXdmNCBJTlBVVCB4LCB5CjUgSUYgKCh4IDwgeSkgJiYgKHggKyA1ICEhIHkpKSBUSEVOIDcKNiBMRVQgeCA9IHggKyAyCjcgTEVUIHkgPSB5ICsgMQo4IEVYSVQgeCArIHkgKiB5CpR0lGUu"

test_cases = pickle.loads(base64.b64decode(testdata.encode()))
info('Test cases count: {}'.format(len(test_cases)))
if len(YOUR_BASEPATH) == 0:
    fatal('Base path error.')

for i in test_cases:
    result = judge(i)
    if not result:
        #print('TEST FAILED')
        #sys.exit(0)
        pass

#print('PASSED ALL')