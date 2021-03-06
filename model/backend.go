package model

type UserGroup interface {
	GetID() int
	GetName() string
	setName(string)
	linkValues() []string
	setLinkValues([]string)
	value() []string
}
type Backends []Backend
type Backend interface {
	// Getter
	FindUserByID(int) (map[string]UserGroup, error)
	FindUserByName(string) (map[string]UserGroup, error)
	FindGroupByID(int) (map[string]UserGroup, error)
	FindGroupByName(string) (map[string]UserGroup, error)
	Users() (map[string]UserGroup, error)
	Groups() (map[string]UserGroup, error)
	HighestUserID() int
	LowestUserID() int
	HighestGroupID() int
	LowestGroupID() int

	// Setter
	CreateUser(UserGroup) error
	DeleteUser(int) error
	UpdateUser(int, UserGroup) error
	CreateGroup(UserGroup) error
	DeleteGroup(int) error
	UpdateGroup(int, UserGroup) error
}

func mergeUserGroup(m1, m2 map[string]UserGroup) map[string]UserGroup {
	ans := map[string]UserGroup{}

	for k, v := range m1 {
		ans[k] = v
	}
	for k, v := range m2 {
		ans[k] = v
	}
	return (ans)
}

func SyncConfig(resourceName string, b Backend, configResources, backendResources map[string]UserGroup) error {
	if configResources != nil {
		for _, cu := range configResources {
			found := false

			if backendResources != nil {
				for _, eu := range backendResources {
					if cu.GetID() == eu.GetID() {
						found = true
						break
					}
				}
			}

			if !found {
				if resourceName == "users" {
					if err := b.CreateUser(cu); err != nil {
						return err
					}

				} else {
					if err := b.CreateGroup(cu); err != nil {
						return err
					}
				}
			}
		}
	}

	if backendResources != nil {
		for _, eu := range backendResources {
			found := false
			if configResources != nil {
				for _, cu := range configResources {
					if cu.GetID() == eu.GetID() {
						found = true
						break
					}
				}
			}
			if !found {
				if resourceName == "users" {
					if err := b.DeleteUser(eu.GetID()); err != nil {
						return err
					}

				} else {
					if err := b.DeleteGroup(eu.GetID()); err != nil {
						return err
					}
				}
			}
		}

	}
	return nil
}
