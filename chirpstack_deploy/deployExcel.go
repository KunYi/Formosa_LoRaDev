package main

//
// the code modify from https://forum.chirpstack.io/t/add-nodes-automatically-to-loraserver/5522/8
// 1'st row is cell title
//	ProfileID, AppId, NAME,	Descriptions,	DevEUI,	AppKey,	strNwkKey, 1GenApplicationKey
//

import (
	"context"
	"crypto/tls"
	"flag"
	"log"
	"strings"

	api "github.com/brocaar/chirpstack-api/go/as/external/api"
	"github.com/pkg/errors"
	"github.com/tealeg/xlsx"
	grpc "google.golang.org/grpc"
	"google.golang.org/grpc/codes"
	"google.golang.org/grpc/credentials"
)

// JWTCredentials provides JWT credentials for gRPC
type JWTCredentials struct {
	token string
}

// GetRequestMetadata returns the meta-data for a request.
func (j *JWTCredentials) GetRequestMetadata(ctx context.Context, url ...string) (map[string]string, error) {
	return map[string]string{
		"authorization": j.token,
	}, nil
}

// RequireTransportSecurity ...
func (j *JWTCredentials) RequireTransportSecurity() bool {
	return false
}

// SetToken sets the JWT token.
func (j *JWTCredentials) SetToken(token string) {
	j.token = token
}

// DeviceImportRecord defines a record for a device to import.
type DeviceImportRecord struct {
	DevEUI          string
	ApplicationID   int64
	DeviceProfileID string
	Name            string
	Description     string
	NetworkKey      string
	ApplicationKey  string
	GenAppKey       string
}

var (
	username       string
	password       string
	file           string
	apiHost        string
	apiInsecure    bool
	jwtCredentials *JWTCredentials
)

func init() {
	jwtCredentials = &JWTCredentials{}

	flag.StringVar(&username, "username", "admin", "LoRa App Server username")
	flag.StringVar(&password, "password", "admin", "LoRa App Server password")
	flag.StringVar(&file, "file", "", "Path to Excel file")
	flag.StringVar(&apiHost, "api", "raspberrypi3.local:8080", "hostname:port to LoRa App Server API")
	flag.BoolVar(&apiInsecure, "api-insecure", false, "LoRa App Server API does not use TLS")
	flag.Parse()
}

func getGRPCConn() (*grpc.ClientConn, error) {
	dialOpts := []grpc.DialOption{
		grpc.WithBlock(),
		grpc.WithPerRPCCredentials(jwtCredentials),
	}

	if apiInsecure {
		log.Println("using insecure api")
		dialOpts = append(dialOpts, grpc.WithInsecure())
	} else {
		dialOpts = append(dialOpts, grpc.WithTransportCredentials(credentials.NewTLS(&tls.Config{
			InsecureSkipVerify: true,
		})))
	}

	conn, err := grpc.Dial(apiHost, dialOpts...)
	if err != nil {
		return nil, errors.Wrap(err, "grpc dial error")
	}

	return conn, nil
}

func login(conn *grpc.ClientConn) error {
	internalClient := api.NewInternalServiceClient(conn)

	resp, err := internalClient.Login(context.Background(), &api.LoginRequest{
		Username: username,
		Password: password,
	})
	if err != nil {
		return errors.Wrap(err, "login error")
	}

	jwtCredentials.SetToken(resp.Jwt)

	return nil
}

func getDeviceImportList() ([]DeviceImportRecord, error) {
	xlFile, err := xlsx.OpenFile(file)
	if err != nil {
		return nil, errors.Wrap(err, "open excel file error")
	}

	var out []DeviceImportRecord

	for _, sheet := range xlFile.Sheets {
		maxRow := sheet.MaxRow
		// log.Println("maxRow:", maxRow)
		i := 1
		for i < maxRow {
			row, err := sheet.Row(i)
			if err != nil {
				log.Fatalf("Get Row failed")
			}

			deviceProfileID := row.GetCell(0).String()
			applicationID, err := row.GetCell(1).Int64()
			if err != nil {
				log.Fatalf("application id parse error (row %d): %s", i+1, err)
			}
			name := strings.TrimSpace(row.GetCell(2).String())
			description := strings.TrimSpace(row.GetCell(3).String())
			devEUI := strings.TrimSpace(row.GetCell(4).String())
			applicationKey := strings.TrimSpace(row.GetCell(5).String())
			networkKey := strings.TrimSpace(row.GetCell(6).String())
			genAppKey := strings.TrimSpace(row.GetCell(7).String())
			// log.Println(fmt.Sprintf("%s, %s, %s, %s, %s", name, devEUI, networkKey, applicationKey, genAppKey))
			out = append(out, DeviceImportRecord{
				DevEUI:          devEUI,
				ApplicationID:   applicationID,
				DeviceProfileID: deviceProfileID,
				Name:            name,
				Description:     description,
				NetworkKey:      networkKey,
				ApplicationKey:  applicationKey,
				GenAppKey:       genAppKey,
			})
			i++
		}
	}
	return out, nil
}

func importDevices(conn *grpc.ClientConn, devices []DeviceImportRecord) error {
	deviceClient := api.NewDeviceServiceClient(conn)

	for i, dev := range devices {
		d := api.Device{
			DevEui:          dev.DevEUI,
			Name:            dev.Name,
			ApplicationId:   dev.ApplicationID,
			Description:     dev.Description,
			DeviceProfileId: dev.DeviceProfileID,
			SkipFCntCheck:   true,
		}

		dk := api.DeviceKeys{
			DevEui:    dev.DevEUI,
			NwkKey:    dev.ApplicationKey,
			AppKey:    dev.NetworkKey,
			GenAppKey: dev.GenAppKey,
		}

		_, err := deviceClient.Create(context.Background(), &api.CreateDeviceRequest{
			Device: &d,
		})
		if err != nil {
			if grpc.Code(err) == codes.AlreadyExists {
				log.Printf("device %s already exists (row %d)", d.DevEui, i+2)
				deviceClient.Delete(context.Background(), &api.DeleteDeviceRequest{
					DevEui: d.GetDevEui(),
				})
				continue
			}
			log.Fatalf("import error (device %s row %d): %s", d.DevEui, i+2, err)
		}

		_, err = deviceClient.CreateKeys(context.Background(), &api.CreateDeviceKeysRequest{
			DeviceKeys: &dk,
		})
		if err != nil {
			if grpc.Code(err) == codes.AlreadyExists {
				log.Printf("device-keys for device %s already exists (row %d)", d.DevEui, i+2)
				continue
			}
			log.Fatalf("import error (device %s) (row %d): %s", d.DevEui, i+2, err)
		}
	}

	return nil
}

func createDeviceProfile(conn *grpc.ClientConn) {
	deviceProfileClient := api.NewDeviceProfileServiceClient(conn)
	deviceProfile := api.DeviceProfile{
		Id:           "FC04C601-A2F7-41A1-B23E-CD4AABA4B05B",
		Name:         "otta",
		SupportsJoin: true,
	}

	_, err := deviceProfileClient.Create(context.Background(), &api.CreateDeviceProfileRequest{
		DeviceProfile: &deviceProfile,
	})

	if err != nil {
		if grpc.Code(err) == codes.AlreadyExists {
			log.Printf("deviceprofile %s already exists", deviceProfile.Id)
		}
		log.Fatalf("import error (deviceprofile %s ): %s", deviceProfile.Id, err)
	}
}

func main() {

	conn, err := getGRPCConn()
	if err != nil {
		log.Fatal("error connecting to api", err)
	}

	if err := login(conn); err != nil {
		log.Fatal("login error", err)
	}

	rows, err := getDeviceImportList()
	if err != nil {
		log.Fatal("get device import records error", err)
	}

	if err := importDevices(conn, rows); err != nil {
		log.Fatal("import error", err)
	}
}
